#pragma once

#include "sparkbox/audio/audio_manager_interface.h"
#include "sparkbox/controller/controller_manager_interface.h"
#include "sparkbox/filesystem/filesystem_manager_interface.h"
#include "sparkbox/status.h"

namespace sparkbox {
// Functions exposed to the device
class SparkboxDeviceInterface {
 public:
  virtual ~SparkboxDeviceInterface() = default;

  virtual Status SetUp() = 0;
  virtual void TearDown() = 0;
  virtual void Start() = 0;
};

// Functions exposed to levels
class SparkboxLevelInterface {
 public:
  virtual ~SparkboxLevelInterface() = default;

  virtual filesystem::FilesystemManagerInterface& Filesystem() = 0;
  virtual audio::AudioManagerInterface& Audio() = 0;
  virtual controller::ControllerManagerInterface& Controller() = 0;
};

}  // namespace sparkbox