#include "sparkbox/audio/audio_manager.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>

#include "FreeRTOS.h"
#include "sparkbox/assert.h"
#include "sparkbox/audio/stream.h"
#include "sparkbox/log.h"
#include "sparkbox/manager.h"
#include "sparkbox/status.h"
#include "task.h"

namespace {
using sparkbox::Message;
using sparkbox::MessageType;
using sparkbox::Status;
using std::placeholders::_1;
}  // namespace

namespace sparkbox::audio {

struct PlayAudioConfig {
  uint8_t stream;
  int number_of_repeats;
};

struct StreamSourceConfig {
  uint8_t stream;
  const char *audio_file;
};

Status AudioManager::SetUp(void) {
  SP_ASSERT(Manager::SetUp() == Status::kOk);

  // Set the driver callbacks
  AudioDriver::Callback cb =
      std::bind(&AudioManager::BlockCompleteCb, this, _1);
  return driver_.SetOnSampleBlockComplete(cb);
}

void AudioManager::TearDown(void) { Manager::TearDown(); }

Status AudioManager::ImportAudioFiles(const std::string &directory) {
  Message message =
      Message(MessageType::kAudioImportFiles,
              const_cast<char *>(directory.c_str()), directory.length());
  return SendInternalMessage(message);
}

Status AudioManager::SetStreamAudioSource(uint8_t stream,
                                          const char *audio_file) {
  StreamSourceConfig config = {
      .stream = stream,
      .audio_file = audio_file,
  };
  Message message = Message(MessageType::kAudioSetStreamSource, &config,
                            sizeof(StreamSourceConfig));
  return SendInternalMessage(message);
}

Status AudioManager::PlayAudio(uint8_t stream, int number_of_repeats) {
  PlayAudioConfig config = {
      .stream = stream,
      .number_of_repeats = number_of_repeats,
  };
  Message message = Message(MessageType::kAudioStartPlayback, &config,
                            sizeof(PlayAudioConfig));
  return SendInternalMessage(message);
}

Status AudioManager::StopAudio(uint8_t stream) {
  Message message =
      Message(MessageType::kAudioStopPlayback, &stream, sizeof(uint8_t));
  return SendInternalMessage(message);
}

// Dispatch a message. Guaranteed to be on the audio manager's task
void AudioManager::HandleMessage(Message &message) {
  if (message.type() == MessageType::kAudioStartPlayback) {
    const PlayAudioConfig *config = message.payload_ptr_as<PlayAudioConfig>();
    HandleAudioStartPlayback(config->stream, config->number_of_repeats);
  } else if (message.type() == MessageType::kAudioStopPlayback) {
    // Stop audio playback for the requested stream
    const uint8_t *stream_idx = message.payload_ptr_as<uint8_t>();
    HandleAudioStopPlayback(*stream_idx);
  } else if (message.type() == MessageType::kAudioBlockComplete) {
    // Just finished sending the last block to audio driver
    HandleAudioBlockComplete();
  } else if (message.type() == MessageType::kAudioSetStreamSource) {
    const StreamSourceConfig *config =
        message.payload_ptr_as<StreamSourceConfig>();
    HandleAudioSetStreamSource(config->stream, config->audio_file);
  } else if (message.type() == MessageType::kAudioImportFiles) {
    const char *directory = message.payload_ptr_as<char>();
    HandleImportAudioFiles(directory);
  } else {
    SP_LOG_INFO("Unknown message type received by audio manager: %zu",
                static_cast<size_t>(message.type()));
  }
}

void AudioManager::HandleImportAudioFiles(const char *directory) {
  for (auto stream = 0; stream < kMaxStreams; stream++) {
    HandleAudioStopPlayback(stream);
  }

  audio_file_importer_.ImportAudioFiles(directory);
}

void AudioManager::HandleAudioStartPlayback(uint8_t stream,
                                            int number_of_repeats) {
  // Invalid channel number, do nothing
  if (stream >= audio_streams_.size()) {
    SP_LOG_ERROR("Invalid channel: %u", stream);
    return;
  }
  bool any_stream_was_playing = AnyStreamPlaying();

  // Reset the repeat count
  audio_streams_[stream].SetRepeats(number_of_repeats);
  // Channel is already playing, reset number of repeats only
  if (audio_streams_[stream].GetPlaybackStatus() ==
      SparkboxStream::PlaybackStatus::kPlaying) {
    SP_LOG_INFO("Audio channel %u is already playing, setting repeats to %d",
                stream, number_of_repeats);
    return;
  } else {
    SP_LOG_DEBUG("Starting playback on stream %u", stream);
  }

  // This stream is being turned on. Start it from the beginning of the file
  audio_streams_[stream].SkipToSampleBlock(0);
  audio_streams_[stream].SetPlaybackStatus(
      SparkboxStream::PlaybackStatus::kPlaying);

  // This is the first stream being turned on
  if (!any_stream_was_playing) {
    // Inform the device it needs to be ready for playback
    driver_.PlaybackStart();
    // Since this is the first buffer, mix it, then immediately send it
    next_buffer_idx_ = 0;
    MixNextBuffer();
    WriteNextBuffer();

    // Then immediately begin mixing the next buffer afterwards to start the
    // cycle
    MixNextBuffer();
  }
}

void AudioManager::HandleAudioStopPlayback(uint8_t channel) {
  // Invalid channel number, do nothing
  if (channel >= audio_streams_.size()) {
    SP_LOG_ERROR("Invalid channel: %u", channel);
    return;
  }

  if (audio_streams_[channel].GetPlaybackStatus() ==
      SparkboxStream::PlaybackStatus::kPlaying) {
    SP_LOG_DEBUG("Stopping channel %u playback", channel);
  }

  // Set the status to stopped. Audio will actually be stopped later
  audio_streams_[channel].SetPlaybackStatus(
      SparkboxStream::PlaybackStatus::kStopped);
}

void AudioManager::HandleAudioBlockComplete(void) {
  // We just finished sending a buffer of audio. If no channels are playing, do
  // not mix any more and let the driver know we're done
  if (!AnyStreamPlaying()) {
    driver_.PlaybackStop();
    return;
  }

  MixNextBuffer();
}

void AudioManager::HandleAudioSetStreamSource(uint8_t stream,
                                              const char *audio_file) {
  if (stream >= audio_streams_.size()) {
    SP_LOG_ERROR("Invalid stream number: %u", stream);
    return;
  }

  ImportedFile *imported_audio_file =
      audio_file_importer_.GetImportedFile(audio_file);
  // Check if the audio file has been imported
  if (imported_audio_file == nullptr) {
    SP_LOG_ERROR("Audio file '%s' not imported", audio_file);
    return;
  }

  audio_streams_[stream].SetPlaybackStatus(
      SparkboxStream::PlaybackStatus::kStopped);
  audio_streams_[stream].SetSource(imported_audio_file);
  SP_LOG_INFO("Set audio stream %u source to '%s'", stream, audio_file);
}

void AudioManager::MixNextBuffer() {
  MixedAudioBuffer &next_mixed_buffer =
      mixed_samples_buffers_[next_buffer_idx_];

  // Do nothing if no stream is playing
  if (!AnyStreamPlaying()) {
    SP_LOG_DEBUG("No more streams playing, stopping mixing");
    return;
  }

  // Find the sample rate - the highest of all active streams
  uint32_t sample_rate_hz = 0;
  for (auto &stream_it : audio_streams_) {
    if (stream_it.GetPlaybackStatus() !=
        SparkboxStream::PlaybackStatus::kPlaying) {
      continue;
    }
    sample_rate_hz = std::max(sample_rate_hz, stream_it.GetSampleRateHz());
  }
  SP_ASSERT(sample_rate_hz != 0);
  next_mixed_buffer.sample_rate_hz_ = sample_rate_hz;

  // Get the next group of samples from each active stream
  size_t stream_active_bitmask = 0;
  for (int stream_idx = 0; stream_idx < kMaxStreams; stream_idx++) {
    if (audio_streams_[stream_idx].GetPlaybackStatus() !=
        SparkboxStream::PlaybackStatus::kPlaying) {
      continue;
    }

    // Save that this stream was active. It may go inactive if it finishes
    // playback
    stream_active_bitmask |= 0x01 << stream_idx;

    // If getting the samples begets an error, stop that stream
    if (audio_streams_[stream_idx].GetNextSamples(
            std::span<int16_t>(unmixed_samples_[stream_idx]), sample_rate_hz) !=
        sparkbox::Status::kOk) {
      SP_LOG_ERROR("Error getting samples for audio stream %d, pausing it",
                   stream_idx);
      audio_streams_[stream_idx].SetPlaybackStatus(
          SparkboxStream::PlaybackStatus::kStopped);
    }
  }

  // Mix the samples into the mixed samples buffer. For each sample, for each
  // stream, add their samples up
  for (size_t samp_idx = 0; samp_idx < next_mixed_buffer.UsedSamplesSize();
       samp_idx++) {
    int16_t sample_out = 0;
    for (size_t str_idx = 0; str_idx < unmixed_samples_.size(); str_idx++) {
      if ((stream_active_bitmask >> str_idx) & 0x01) {
        sample_out += unmixed_samples_[str_idx][samp_idx];
      }
    }
    next_mixed_buffer.mixed_samples_[samp_idx] = sample_out;
  }

  next_mixed_buffer.is_ready_ = true;
}

// Send the next batch of samples to the driver as described by next_buffer_
void AudioManager::WriteNextBuffer() {
  // If the next buffer has no streams playing, audio will be stopped later. Do
  // nothing about it here
  MixedAudioBuffer &next_mixed_buffer =
      mixed_samples_buffers_[next_buffer_idx_];

  // If we're not ready, there was not enough time to fill this buffer
  SP_ASSERT(next_mixed_buffer.is_ready_);

  // Send the collection of filled samples to the driver to play
  std::span<int16_t> samples =
      std::span<int16_t>(next_mixed_buffer.mixed_samples_.data(),
                         next_mixed_buffer.UsedSamplesSize());
  driver_.WriteSampleBlock(samples, next_mixed_buffer.sample_rate_hz_);

  // Increment the next buffer index and set it to not ready
  next_buffer_idx_ = (next_buffer_idx_ + 1) % kNumMixedBuffers;
  next_mixed_buffer.is_ready_ = false;
}

// Last block of audio was sent by the device driver. This is in an interrupt
// context
void AudioManager::BlockCompleteCb(Status status) {
  if (status != Status::kOk) {
    SP_LOG_ERROR("Error writing audio block: %d. Continuing...",
                 static_cast<int>(status));
  }

  // A block just finished sending, immediately send the next buffer
  WriteNextBuffer();

  // Let ourselves know the buffer finished sending so we can populate the next
  // buffer. This will be processed on the audio task
  Message message = Message(MessageType::kAudioBlockComplete);
  SendInternalMessageISR(message);
}

}  // namespace sparkbox::audio