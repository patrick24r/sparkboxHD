#include "sparkbox/audio/channel.h"

#include <span>

#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace sparkbox::audio {

Status Channel::SetSource(ImportedFile* file) {
  if (file == nullptr) {
    return sparkbox::Status::kBadParameter;
  } else if (playback_status_ != PlaybackStatus::kStopped) {
    return sparkbox::Status::kBadResourceState;
  }
  audio_source_ = file;
  next_sample_index_ = 0;
  repeat_count_ = 0;
  return sparkbox::Status::kOk;
}

Status Channel::SkipToSample(size_t sample_index) {
  if (audio_source_ == nullptr) {
    return sparkbox::Status::kBadResourceState;
  }
  next_sample_index_ = sample_index;

  return sparkbox::Status::kOk;
}

Status Channel::SkipToTimeMicroseconds(size_t microseconds) {
  if (audio_source_ == nullptr) {
    return sparkbox::Status::kBadResourceState;
  }
  size_t sample_index =
      microseconds * audio_source_->sample_rate_hz() / 1000000;
  return SkipToSample(sample_index);
}

// Get the next batch of samples for the given audio channel
Status Channel::GetNextSamples(std::span<int16_t> samples, bool is_mono,
                               uint32_t sample_rate) {
  // Calculate resample fraction
  resampler_.GetResampleFilter();

  // Keep getting samples until we finish number of repeats or fill the buffer
  while () {
    resampler_.ResampleNextBlock();
  }

  return Status::kOk;
}

}  // namespace sparkbox::audio