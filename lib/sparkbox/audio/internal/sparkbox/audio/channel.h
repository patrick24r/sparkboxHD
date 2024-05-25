#pragma once

#include <cstdint>
#include <span>

#include "sparkbox/audio/imported_file.h"
#include "sparkbox/audio/resampler.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

// Represents an audio channel
class Channel {
 public:
  // Set the file source. Only valid if audio on this channel is stopped
  sparkbox::Status SetSource(ImportedFile* file);

  sparkbox::Status SkipToSample(size_t sample_index);
  sparkbox::Status SkipToTimeMicroseconds(size_t microseconds);

  sparkbox::Status GetNextSamples(std::span<int16_t> samples, bool is_mono,
                                  uint32_t sample_rate);

  void SetRepeatCount(int repeat_count) { repeat_count_ = repeat_count; }

  enum PlaybackStatus {
    kPlaying,
    kStopped,
  };
  void SetPlaybackStatus(PlaybackStatus status) { playback_status_ = status; }
  PlaybackStatus GetPlaybackStatus(void) { return playback_status_; }
  uint32_t GetSampleRate(void) {
    if (audio_source_ == nullptr) return 0;
    return audio_source_->sample_rate_hz();
  }
  uint8_t GetNumberOfChannels(void) {
    if (audio_source_ == nullptr) return 0;
    return audio_source_->number_of_channels();
  }

 private:
  Resampler resampler_;
  Resampler::ResampleFilter filter_;

  ImportedFile* audio_source_ = nullptr;
  PlaybackStatus playback_status_ = PlaybackStatus::kStopped;
  size_t next_source_sample_index_ = 0;
  int repeat_count_ = 0;
};

}  // namespace sparkbox::audio