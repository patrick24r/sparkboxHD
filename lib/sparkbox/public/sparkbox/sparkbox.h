#pragma once

#include "sparkbox/audio/audio_manager.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_manager.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core_driver.h"
#include "sparkbox/filesystem/filesystem_manager.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/status.h"

namespace {
using ::sparkbox::controller::ControllerManager;
}

namespace sparkbox {

class Sparkbox final {
 public:
  Sparkbox(CoreDriver& core_driver,
           filesystem::FilesystemDriver& fs_driver,
           controller::ControllerDriver& cont_driver) :
    core_driver_(core_driver),
    fs_manager_{fs_driver},
    controller_manager_{cont_driver} {}

  Status SetUp(void);
  void TearDown(void);

  // Start the sparkbox
  void Start(void);

  filesystem::FilesystemManager& Filesystem(void) { return fs_manager_; }
  controller::ControllerManager& Controller(void) { return controller_manager_; }

 private:
  CoreDriver& core_driver_;
  filesystem::FilesystemManager fs_manager_;
  controller::ControllerManager controller_manager_;
  
};

} // namespace Sparkbox