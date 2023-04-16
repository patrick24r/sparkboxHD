#pragma once

#include "sparkbox/status.h"

namespace sparkbox::audio
{

class AudioController {
 public:
  sparkbox::Status ImportAudioFiles(const char * directory);
  sparkbox::Status ImportAudioFile(const char * file);

 private:
  sparkbox::Status ImportWavFile(const char * file);
  sparkbox::Status ImportMp3File(const char * file);
};


} // namespace Sparkbox::Audio
