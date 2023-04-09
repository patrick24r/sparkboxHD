#pragma once

#include "filesystem/filesystem_controller.h"
#include "audio/audio_controller.h"

namespace {
using ::Sparkbox::Audio;
using ::Sparkbox::Filesystem;
}

namespace Sparkbox {

class Sparkbox final {
 public:
  Sparkbox();

 private:
  Audio::AudioController audio_;
  Filesystem::FilesystemController fs_;
};

} // namespace Sparkbox