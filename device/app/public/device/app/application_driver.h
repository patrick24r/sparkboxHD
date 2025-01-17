#pragma once

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace device::app {

// Methods the application needs to call on drivers, but sparkbox does not
class ApplicationDriver {
 public:
  virtual void SetUp() = 0;
  virtual void TearDown() = 0;
};

class CoreAppDriver : public ApplicationDriver, public sparkbox::CoreDriver {};
class FilesystemAppDriver : public ApplicationDriver,
                            public sparkbox::filesystem::FilesystemDriver {};
class AudioAppDriver : public ApplicationDriver,
                       public sparkbox::audio::AudioDriver {};
class ControllerAppDriver : public ApplicationDriver,
                            public sparkbox::controller::ControllerDriver {};

}  // namespace device::app
