#pragma once

#include "sparkbox/audio/audio_manager.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_manager.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/filesystem/filesystem_manager.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace {
using namespace ::sparkbox::audio;
using namespace ::sparkbox::filesystem;
}

namespace sparkbox {

class Sparkbox final {
 public:
  Sparkbox(filesystem::FilesystemDriver& fs_driver,
           controller::ControllerDriver& cont_driver) :
    controller_manager_{cont_driver},
    fs_manager_{fs_driver} {}

  controller::ControllerManager& Controller(void) { return controller_manager_; }
  filesystem::FilesystemManager& Filesystem(void) { return fs_manager_; }


 private:
  controller::ControllerManager controller_manager_;
  filesystem::FilesystemManager fs_manager_;
};

} // namespace Sparkbox