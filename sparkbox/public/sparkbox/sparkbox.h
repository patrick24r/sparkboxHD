#pragma once

#include "sparkbox/audio/audio_controller.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_controller.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/filesystem/filesystem_controller.h"
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
    cont_controller_{cont_driver},
    fs_controller_{fs_driver} {}

  controller::ControllerController& Controller(void) { return cont_controller_; }
  filesystem::FilesystemController& Filesystem(void) { return fs_controller_; }


 private:
  controller::ControllerController cont_controller_;
  filesystem::FilesystemController fs_controller_;
};

} // namespace Sparkbox