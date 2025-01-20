#pragma once

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/audio/audio_manager.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_manager.h"
#include "sparkbox/core/core_driver.h"
#include "sparkbox/core/core_manager.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/filesystem/filesystem_manager.h"
#include "sparkbox/router.h"
#include "sparkbox/sparkbox_interface.h"
#include "sparkbox/status.h"

namespace sparkbox {

class Sparkbox : public SparkboxDeviceInterface, public SparkboxLevelInterface {
 public:
  Sparkbox(CoreDriver& core_driver, filesystem::FilesystemDriver& fs_driver,
           audio::AudioDriver& audio_driver,
           controller::ControllerDriver& cont_driver)
      : router_(),
        core_manager_{*this, core_driver},
        fs_manager_{fs_driver},
        audio_manager_{router_, audio_driver, fs_driver},
        controller_manager_{router_, cont_driver},
        entry_task_("SparkboxEntryTask", EntryTaskWrapper) {}

  Status SetUp(void) final;
  void TearDown(void) final;

  // Start the sparkbox
  void Start() final;

  // SparkboxLevelInterface functions
  audio::AudioManagerInterface& Audio(void) final { return audio_manager_; }
  filesystem::FilesystemManagerInterface& Filesystem(void) final {
    return fs_manager_;
  }
  controller::ControllerManagerInterface& Controller(void) final {
    return controller_manager_;
  }

 private:
  Router router_;
  CoreManager core_manager_;
  filesystem::FilesystemManager fs_manager_;
  audio::AudioManager audio_manager_;
  controller::ControllerManager controller_manager_;

  // Entry task for the sparkbox
  Task entry_task_;
  static void EntryTaskWrapper(void* sparkbox_ptr);
  void EntryTask();
};

}  // namespace sparkbox