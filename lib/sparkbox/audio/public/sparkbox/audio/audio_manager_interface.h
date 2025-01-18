#pragma once

#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManagerInterface {
 public:
  // Imports all audio in a given directory, overwriting any pre-imported files.
  // Stops all playback
  virtual sparkbox::Status ImportAudioFiles(const std::string& directory) = 0;
  // Set the audio file source for a channel. Stops any
  // ongoing playback on that channel
  virtual sparkbox::Status SetChannelAudioSource(uint8_t channel,
                                                 const char* audio_file) = 0;
  virtual sparkbox::Status PlayAudio(uint8_t channel,
                                     int number_of_repeats) = 0;
  virtual sparkbox::Status StopAudio(uint8_t channel) = 0;
};

}  // namespace sparkbox::audio
