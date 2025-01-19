#include "sparkbox/audio/audio_manager.h"

#include <algorithm>
#include <cstdint>
#include <cstring>
#include <span>

#include "FreeRTOS.h"
#include "sparkbox/assert.h"
#include "sparkbox/audio/channel.h"
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
  uint8_t channel;
  int number_of_repeats;
};

struct ChannelSourceConfig {
  uint8_t channel;
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

Status AudioManager::SetChannelAudioSource(uint8_t channel,
                                           const char *audio_file) {
  ChannelSourceConfig config = {
      .channel = channel,
      .audio_file = audio_file,
  };
  Message message = Message(MessageType::kAudioSetChannelSource, &config,
                            sizeof(ChannelSourceConfig));
  return SendInternalMessage(message);
}

Status AudioManager::PlayAudio(uint8_t channel, int number_of_repeats) {
  PlayAudioConfig config = {
      .channel = channel,
      .number_of_repeats = number_of_repeats,
  };
  Message message = Message(MessageType::kAudioStartPlayback, &config,
                            sizeof(PlayAudioConfig));
  return SendInternalMessage(message);
}

Status AudioManager::StopAudio(uint8_t channel) {
  Message message =
      Message(MessageType::kAudioStopPlayback, &channel, sizeof(uint8_t));
  return SendInternalMessage(message);
}

// Dispatch a message. Guaranteed to be on the audio manager's task
void AudioManager::HandleMessage(Message &message) {
  if (message.type() == MessageType::kAudioStartPlayback) {
    const PlayAudioConfig *config = message.payload_ptr_as<PlayAudioConfig>();
    HandleAudioStartPlayback(config->channel, config->number_of_repeats);
  } else if (message.type() == MessageType::kAudioStopPlayback) {
    // Stop audio playback for the requested channel
    const uint8_t *channel = message.payload_ptr_as<uint8_t>();
    HandleAudioStopPlayback(*channel);
  } else if (message.type() == MessageType::kAudioBlockComplete) {
    // Just finished sending the last block to audio driver
    HandleAudioBlockComplete();
  } else if (message.type() == MessageType::kAudioSetChannelSource) {
    const ChannelSourceConfig *config =
        message.payload_ptr_as<ChannelSourceConfig>();
    HandleAudioSetChannelSource(config->channel, config->audio_file);
  } else if (message.type() == MessageType::kAudioImportFiles) {
    const char *directory = message.payload_ptr_as<char>();
    HandleImportAudioFiles(directory);
  } else {
    SP_LOG_INFO("Unknown message type received by audio manager: %zu",
                static_cast<size_t>(message.type()));
  }
}

void AudioManager::HandleImportAudioFiles(const char *directory) {
  for (auto channel = 0; channel < kMaxChannels; channel++) {
    StopAudio(channel);
  }

  audio_file_importer_.ImportAudioFiles(directory);
}

void AudioManager::HandleAudioStartPlayback(uint8_t channel,
                                            int number_of_repeats) {
  // Invalid channel number, do nothing
  if (channel >= audio_channel_.size()) {
    SP_LOG_ERROR("Invalid channel: %u", channel);
    return;
  }
  bool any_channel_was_playing = AnyChannelPlaying();

  // Reset the repeat count
  audio_channel_[channel].SetRepeatCount(number_of_repeats);
  // Channel is already playing, reset number of repeats only
  if (audio_channel_[channel].GetPlaybackStatus() ==
      Channel::PlaybackStatus::kPlaying) {
    SP_LOG_INFO("Audio channel %u is already playing", channel);
    return;
  }

  // This channel is being turned on. Start it from the beginning of the file
  audio_channel_[channel].SkipToSample(0);
  audio_channel_[channel].SetPlaybackStatus(Channel::PlaybackStatus::kPlaying);

  // This is the first channel being turned on
  if (!any_channel_was_playing) {
    // Inform the device it needs to be ready for playback
    driver_.PlaybackStart();
    // Since this is the first block, mix the first block then send it
    MixNextSampleBlock();
    // Force any_channel_playing to be true
    next_buffer_.any_channel_playing = true;
    WriteNextSampleBlock();

    // Then immediately begin mixing the next block
    next_buffer_.is_ready = false;
    next_buffer_.buffer_index = (next_buffer_.buffer_index + 1) % 2;
    MixNextSampleBlock();
  }
}

void AudioManager::HandleAudioStopPlayback(uint8_t channel) {
  // Invalid channel number, do nothing
  if (channel >= audio_channel_.size()) {
    SP_LOG_ERROR("Invalid channel: %u", channel);
    return;
  }

  SP_LOG_INFO("Stopping channel %u playback", channel);
  // Set the status to stopped. Audio will actually be stopped later
  audio_channel_[channel].SetPlaybackStatus(Channel::PlaybackStatus::kStopped);
}

void AudioManager::HandleAudioBlockComplete(void) {
  // The last block of samples just finished sending. Set up next_buffer_
  // and mix the next set of samples
  next_buffer_.is_ready = false;
  next_buffer_.buffer_index = (next_buffer_.buffer_index + 1) % 2;
  next_buffer_.any_channel_playing = AnyChannelPlaying();

  // No channels are playing, do not mix the next block and let the driver know
  // audio is done
  if (!next_buffer_.any_channel_playing) {
    driver_.PlaybackStop();
    return;
  }

  MixNextSampleBlock();
}

void AudioManager::HandleAudioSetChannelSource(uint8_t channel,
                                               const char *audio_file) {
  if (channel >= audio_channel_.size()) {
    SP_LOG_ERROR("Invalid channel number: %u", channel);
    return;
  }

  ImportedFile *imported_audio_file =
      audio_file_importer_.GetImportedFile(audio_file);
  // Check if the audio file has been imported
  if (imported_audio_file == nullptr) {
    SP_LOG_ERROR("Audio file '%s' not imported", audio_file);
    return;
  }

  audio_channel_[channel].SetPlaybackStatus(Channel::PlaybackStatus::kStopped);
  audio_channel_[channel].SetSource(imported_audio_file);
  SP_LOG_INFO("Set audio channel %u source to '%s'", channel, audio_file);
}

void AudioManager::MixNextSampleBlock(void) {
  // Find the highest resolution sample rate and stereo/mono among all channels
  next_buffer_.is_mono = true;
  next_buffer_.sample_rate_hz = 1;
  for (uint8_t channel_idx = 0; channel_idx < audio_channel_.size();
       channel_idx++) {
    if (audio_channel_[channel_idx].GetPlaybackStatus() !=
        Channel::PlaybackStatus::kPlaying) {
      continue;
    }

    next_buffer_.sample_rate_hz =
        std::max(next_buffer_.sample_rate_hz,
                 audio_channel_[channel_idx].GetSampleRate());
    if (audio_channel_[channel_idx].GetNumberOfChannels() > 1) {
      next_buffer_.is_mono = false;
    }
  }

  // Calculate how many samples to ask each channel for
  next_buffer_.buffer_size_samples =
      (kBufferDepthMs * next_buffer_.sample_rate_hz *
       (next_buffer_.is_mono ? 1 : 2)) /
      1000;

  // Get the next group of samples from each channel
  for (uint8_t channel_idx = 0; channel_idx < audio_channel_.size();
       channel_idx++) {
    if (audio_channel_[channel_idx].GetPlaybackStatus() !=
        Channel::PlaybackStatus::kPlaying) {
      continue;
    }

    // Get the samples from each channel into temporary storage
    if (audio_channel_[channel_idx].GetNextSamples(
            std::span<int16_t>(unmixed_samples_buffer_[channel_idx].begin(),
                               next_buffer_.buffer_size_samples),
            next_buffer_.is_mono, next_buffer_.sample_rate_hz) != Status::kOk) {
      SP_LOG_ERROR("Error getting samples for audio channel %u", channel_idx);
    }
  }

  // Mix samples into the proper buffer half
  for (uint32_t sample_idx = 0; sample_idx < next_buffer_.buffer_size_samples;
       sample_idx++) {
    mixed_samples_buffer_[next_buffer_.buffer_index][sample_idx] = 0;
    for (uint8_t channel_idx = 0; channel_idx < audio_channel_.size();
         channel_idx++) {
      if (audio_channel_[channel_idx].GetPlaybackStatus() !=
          Channel::PlaybackStatus::kPlaying) {
        continue;
      }

      // When mixing, divide the volume by the number of channels to prevent
      // clipping
      mixed_samples_buffer_[next_buffer_.buffer_index][sample_idx] +=
          unmixed_samples_buffer_[channel_idx][sample_idx];
      // /audio_channel_.size();
    }
  }

  next_buffer_.is_ready = true;
}

// Send the next batch of samples to the driver as described by next_buffer_
void AudioManager::WriteNextSampleBlock(void) {
  // If the next buffer has no channels playing, audio will be stopped later
  if (!next_buffer_.any_channel_playing) {
    return;
  }

  // If we're not ready, there was not enough time to fill this buffer
  SP_ASSERT(next_buffer_.is_ready);

  // Send the collection of filled samples to the driver to play
  std::span<int16_t> samples = std::span<int16_t>(
      mixed_samples_buffer_[next_buffer_.buffer_index].data(),
      next_buffer_.buffer_size_samples);
  driver_.WriteSampleBlock(samples, next_buffer_.is_mono,
                           next_buffer_.sample_rate_hz);
}

// Last block of audio was sent by the device driver. This is in an interrupt
// context
void AudioManager::BlockCompleteCb(Status status) {
  // A block just finished sending, immediately send the next buffer
  WriteNextSampleBlock();

  // Let ourselves know the buffer finished sending so we can populate the next
  // buffer. This will be processed on the audio task
  Message message = Message(MessageType::kAudioBlockComplete);
  SendInternalMessageISR(message);
}

}  // namespace sparkbox::audio