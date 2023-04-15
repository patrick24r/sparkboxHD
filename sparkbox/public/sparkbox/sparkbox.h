#pragma once

#include "sparkbox/audio/audio_controller.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/filesystem/filesystem_controller.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using namespace ::sparkbox::audio;
using namespace ::sparkbox::filesystem;
}

namespace sparkbox {

class Sparkbox final {
 public:
  Sparkbox(filesystem::FilesystemDriver& fs_driver) :
    fs_controller_{fs_driver} {}

  filesystem::FilesystemController&
    GetFilesystemController() { return fs_controller_; }

 private:
  filesystem::FilesystemController fs_controller_;
};


// Logging macros
#define SP_LOG_ERROR
#define SP_LOG_WARNING
#define SP_LOG_INFO
#define SP_LOG_DEBUG



} // namespace Sparkbox