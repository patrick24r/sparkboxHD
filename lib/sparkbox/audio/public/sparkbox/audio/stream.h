#pragma once

#include <array>
#include <cstdint>
#include <span>

#include "sparkbox/audio/imported_file.h"
#include "sparkbox/audio/resampler.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

// Represents an audio stream
template <size_t MaxSamplesSize>
class Stream {
 public:
  enum PlaybackStatus : uint8_t {
    kPlaying,
    kStopped,
  };

  static constexpr int kInfiniteRepeats = -1;

  // Set the file source. Only valid if audio on this channel is stopped
  sparkbox::Status SetSource(ImportedFile* source);

  // Skip to a specific sample block
  sparkbox::Status SkipToSampleBlock(size_t sample_index);
  sparkbox::Status SkipToTimeMicroseconds(size_t microseconds);

  void SetRepeats(int repeat_count) { repeats_remaining_ = repeat_count; }
  void SetPlaybackStatus(PlaybackStatus status) { playback_status_ = status; }
  PlaybackStatus GetPlaybackStatus() { return playback_status_; }
  uint32_t GetSampleRateHz() {
    if (audio_source_ == nullptr) return 0;
    return audio_source_->sample_rate_hz_;
  }
  uint8_t GetNumberOfChannels() {
    if (audio_source_ == nullptr) return 0;
    return audio_source_->number_of_channels_;
  }

  // Get next batch of samples
  sparkbox::Status GetNextSamples(std::span<int16_t> samples,
                                  uint32_t sample_rate_hz);

 private:
  Resampler::ResampleFilter filter_;
  uint32_t previous_sample_rate_hz_;
  ImportedFile* audio_source_ = nullptr;
  PlaybackStatus playback_status_ = PlaybackStatus::kStopped;
  size_t next_source_block_index_ = 0;
  int repeats_remaining_ = 0;
};

}  // namespace sparkbox::audio