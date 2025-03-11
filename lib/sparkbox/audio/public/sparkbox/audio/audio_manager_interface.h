#pragma once

#include <cstdint>
#include <string>

#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManagerInterface {
 public:
  virtual ~AudioManagerInterface() = default;
  // Imports all audio in a given directory, overwriting any pre-imported files.
  // Stops all playback
  virtual sparkbox::Status ImportAudioFiles(const std::string& directory) = 0;
  // Set the audio file source for a stream. Invalid if the stream is playing
  virtual sparkbox::Status SetStreamAudioSource(uint8_t stream,
                                                const char* audio_file) = 0;
  virtual sparkbox::Status PlayAudio(uint8_t stream, int number_of_repeats) = 0;
  virtual sparkbox::Status StopAudio(uint8_t stream) = 0;
};

}  // namespace sparkbox::audio
