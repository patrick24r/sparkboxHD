#pragma once

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/status.h"

namespace sparkbox::audio {

class AudioManager {
 public:
  AudioManager(AudioDriver &driver) : driver_(driver) {}
  sparkbox::Status ImportAudioFiles(const char *directory);
  sparkbox::Status ImportAudioFile(const char *file);

 private:
  AudioDriver& driver_;

  sparkbox::Status ImportWavFile(const char *file);
  sparkbox::Status ImportMp3File(const char *file);
};


} // namespace Sparkbox::asudio
