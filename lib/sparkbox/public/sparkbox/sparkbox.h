#pragma once

#include "sparkbox/audio/audio_manager.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_manager.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/filesystem/filesystem_manager.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace {
using ::sparkbox::controller::ControllerDriver;
using ::sparkbox::controller::ControllerManager;
using ::sparkbox::filesystem::FilesystemDriver;
using ::sparkbox::filesystem::FilesystemManager;
using ::sparkbox::Status;
}

namespace sparkbox {

class Sparkbox final {
 public:
  Sparkbox(FilesystemDriver& fs_driver,
           ControllerDriver& cont_driver) :
    fs_manager_{fs_driver},
    controller_manager_{cont_driver} {}

  Status SetUp(void);
  void TearDown(void);

  FilesystemManager& Filesystem(void) { return fs_manager_; }
  ControllerManager& Controller(void) { return controller_manager_; }

 private:
  FilesystemManager fs_manager_;
  ControllerManager controller_manager_;
  
};

} // namespace Sparkbox