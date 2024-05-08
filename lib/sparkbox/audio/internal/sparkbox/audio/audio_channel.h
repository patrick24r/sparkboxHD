#pragma once

#include <cstdint>
#include <span>

#include "sparkbox/audio/imported_file.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

// Represents an audio channel
class AudioChannel {
 public:
  enum PlaybackStatus {
    kPlaying,
    kStopped,
  };

  // Set the file source. Only valid if audio on this channel is stopped
  sparkbox::Status SetSource(ImportedFile* file) {
    if (file == nullptr) {
      return sparkbox::Status::kBadParameter;
    } else if (playback_status_ != PlaybackStatus::kStopped) {
      return sparkbox::Status::kBadResourceState;
    }
    audio_source_ = file;
    next_sample_index_ = 0;
    return sparkbox::Status::kOk;
  }

  sparkbox::Status SkipToSample(size_t sample_index) {
    if (audio_source_ == nullptr) {
      return sparkbox::Status::kBadResourceState;
    }
    next_sample_index_ = sample_index;
  }
  sparkbox::Status SkipToTimeMicroseconds(size_t microseconds) {
    if (audio_source_ == nullptr) {
      return sparkbox::Status::kBadResourceState;
    }
    size_t sample_index =
        microseconds * audio_source_->sample_rate_hz() / 1000000;
    return SkipToSample(sample_index);
  }

  void SetRepeatCount(int repeat_count) { repeat_count_ = repeat_count; }
  void SetPlaybackStatus(PlaybackStatus status) {}

 private:
  ImportedFile* audio_source_ = nullptr;
  PlaybackStatus playback_status_ = PlaybackStatus::kStopped;
  size_t next_sample_index_ = 0;
  int repeat_count_ = 0;
};

}  // namespace sparkbox::audio