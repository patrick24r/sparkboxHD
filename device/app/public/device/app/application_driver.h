#pragma once

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/sparkbox_interface.h"


namespace device::app {

// Methods the application needs to call on drivers, but sparkbox does not
class ApplicationDriver {
 public:
  virtual void SetUp() = 0;
  virtual void TearDown() = 0;
};

class CoreAppDriver : public ApplicationDriver, public sparkbox::CoreDriver {
 public:
  virtual void* LoadSparkbox() = 0;
  virtual sparkbox::SparkboxInterface* CreateSparkbox(
      void* handle, sparkbox::CoreDriver& core_driver,
      sparkbox::filesystem::FilesystemDriver& fs_driver,
      sparkbox::audio::AudioDriver& audio_driver,
      sparkbox::controller::ControllerDriver& controller_driver) = 0;
  virtual void UnloadSparkbox(void* handle) = 0;
};
class FilesystemAppDriver : public ApplicationDriver,
                            public sparkbox::filesystem::FilesystemDriver {};
class AudioAppDriver : public ApplicationDriver,
                       public sparkbox::audio::AudioDriver {};
class ControllerAppDriver : public ApplicationDriver,
                            public sparkbox::controller::ControllerDriver {};

}  // namespace device::app
