#pragma once

#include "sparkbox/audio/audio_manager_interface.h"
#include "sparkbox/controller/controller_manager_interface.h"
#include "sparkbox/filesystem/filesystem_manager_interface.h"
#include "sparkbox/status.h"

namespace sparkbox {
// Functions exposed to library users
class SparkboxInterface {
 public:
  // Functions to be used by the device
  virtual Status SetUp() = 0;
  virtual void TearDown() = 0;
  virtual void Start() = 0;

  // Functions to be used by levels
  virtual filesystem::FilesystemManagerInterface& Filesystem() = 0;
  virtual audio::AudioManagerInterface& Audio() = 0;
  virtual controller::ControllerManagerInterface& Controller() = 0;
};

}  // namespace sparkbox