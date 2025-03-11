#include "sparkbox/audio/stream.h"

#include <span>

#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace sparkbox::audio {

template class Stream<1440>;

template <size_t MaxSamplesSize>
Status Stream<MaxSamplesSize>::SetSource(ImportedFile* source) {
  if (source == nullptr) {
    return sparkbox::Status::kBadParameter;
  } else if (playback_status_ != PlaybackStatus::kStopped) {
    SP_LOG_ERROR(
        "Cannot change audio source for stream while the stream is active");
    return sparkbox::Status::kBadResourceState;
  }

  audio_source_ = source;
  next_source_block_index_ = 0;
  repeats_remaining_ = 0;
  filter_.ResetData();
  return sparkbox::Status::kOk;
}

template <size_t MaxSamplesSize>
Status Stream<MaxSamplesSize>::SkipToSampleBlock(size_t sample_block_index) {
  if (audio_source_ == nullptr) {
    return sparkbox::Status::kBadResourceState;
  }
  next_source_block_index_ = sample_block_index;
  filter_.ResetData();
  return sparkbox::Status::kOk;
}

template <size_t MaxSamplesSize>
Status Stream<MaxSamplesSize>::SkipToTimeMicroseconds(size_t microseconds) {
  size_t sample_index = microseconds * audio_source_->sample_rate_hz_ /
                        1000000 * audio_source_->number_of_channels_;
  return SkipToSampleBlock(sample_index);
}

// Get the next batch of samples for the given audio channel. Samples out are
// always formatted as 16 bit stereo
template <size_t MaxSamplesSize>
Status Stream<MaxSamplesSize>::GetNextSamples(std::span<int16_t> samples_out,
                                              uint32_t sample_rate_hz) {
  if (audio_source_ == nullptr) {
    SP_LOG_ERROR("Audio source not set");
    return Status::kBadParameter;
  }
  // Caller should be expecting stereo, 16 bit audio to be populated in the
  // span's data. samples size should be even
  SP_ASSERT(samples_out.size() % 2 == 0);

  // Do not allow loss of data precision. Cannot go down in sample rate
  SP_ASSERT(sample_rate_hz <= audio_source_->sample_rate_hz_);

  // Do not allow loss of data precision. Cannot go down in byte width
  SP_ASSERT(audio_source_->bytes_per_sample_ <= 2);

  // If the new sample rate does not match the old, get a new filter here
  if (sample_rate_hz != previous_sample_rate_hz_) {
    filter_ = Resampler::GetResampleFilter(sample_rate_hz,
                                           audio_source_->sample_rate_hz_);
  }

  // Copy the audio from the source to samples_out until it's fulfilled.
  // A "block" for the output is 2 samples (L and R channel) of 16 bit audio
  size_t samples_out_total_blocks = samples_out.size() / 2;
  size_t samples_source_total_blocks = samples_out_total_blocks *
                                       audio_source_->sample_rate_hz_ /
                                       sample_rate_hz;

  // Copy all the samples we need from the source to the output span one block
  // at a time, forcing byte width to 2 and channel count to 2
  size_t samples_out_block_idx = 0;
  for (size_t src_copied_blocks = 0;
       src_copied_blocks < samples_source_total_blocks; src_copied_blocks++) {
    if (repeats_remaining_ == 0) {
      // Done playing the audio source, set the remaining samples to 0 and
      // stop sampling
      for (auto& sample_out : samples_out.subspan(samples_out_block_idx * 2)) {
        sample_out = 0;
      }
      break;
    }

    // Copy the block from the source to the output, converting to 16 bit stereo
    int16_t sample_left = 0;
    int16_t sample_right = 0;
    if (audio_source_->bytes_per_sample_ == 1) {
      // 8 bit src
      int8_t* src_bytes = audio_source_->bytes_as<int8_t*>();
      sample_left =
          static_cast<int16_t>(src_bytes[next_source_block_index_ *
                                         audio_source_->number_of_channels_])
          << 8;

      sample_right = sample_left;
      if (audio_source_->number_of_channels_ == 2) {
        sample_right = static_cast<int16_t>(
                           src_bytes[next_source_block_index_ *
                                         audio_source_->number_of_channels_ +
                                     1])
                       << 8;
      }
    } else {
      // 16 bit src
      int16_t* src_bytes = audio_source_->bytes_as<int16_t*>();
      sample_left = src_bytes[next_source_block_index_ *
                              audio_source_->number_of_channels_];
      sample_right = sample_left;
      if (audio_source_->number_of_channels_ == 2) {
        sample_right = src_bytes[next_source_block_index_ *
                                     audio_source_->number_of_channels_ +
                                 1];
      }
    }
    samples_out[samples_out_block_idx * 2] = sample_left;
    samples_out[samples_out_block_idx * 2 + 1] = sample_right;

    samples_out_block_idx++;
    next_source_block_index_++;

    // If we just used the last block of the audio source, restart the track if
    // there are any more repeats. Negative repeat value means infinte repeats
    if (next_source_block_index_ > audio_source_->block_count()) {
      next_source_block_index_ = 0;
      if (repeats_remaining_ > 0) {
        repeats_remaining_ -= 1;
      }
      if (repeats_remaining_ == 0) {
        playback_status_ = PlaybackStatus::kStopped;
      }
    }
  }

  // Resample audio to the new frequency
  Resampler::ResampleNextBlock(
      filter_, samples_out.subspan(0, samples_source_total_blocks * 2), 2,
      samples_out);

  previous_sample_rate_hz_ = sample_rate_hz;
  return Status::kOk;
}

}  // namespace sparkbox::audio