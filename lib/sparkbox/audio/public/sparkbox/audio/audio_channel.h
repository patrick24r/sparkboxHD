#pragma once

#include "sparkbox/audio/imported_file.h"

namespace sparkbox::audio {

// Represents an audio channel
class AudioChannel {
 public:
  void SkipToSample(size_t sample_index);
  void SkipToTimeMicroseconds(size_t microseconds);

 private:
  ImportedFile& audio_source_;

  float volume_;
  float balance_;
};

}  // namespace sparkbox::audio