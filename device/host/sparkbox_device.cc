#include "sparkbox_device.h"

#include "device/app/application_driver.h"
#include "dynamic_loader.h"
#include "host_audio_driver.h"
#include "host_controller_driver.h"
#include "host_core_driver.h"
#include "host_filesystem_driver.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/sparkbox_interface.h"

namespace device {

// Getters for static device drivers
host::CoreDriver core_driver;
host::HostFilesystemDriver filesystem_driver;
host::HostAudioDriver audio_driver;
host::HostControllerDriver controller_driver;

app::CoreAppDriver& GetCoreDriver() { return core_driver; }
app::FilesystemAppDriver& GetFilesystemDriver() { return filesystem_driver; }
app::AudioAppDriver& GetAudioDriver() { return audio_driver; }
app::ControllerAppDriver& GetControllerDriver() { return controller_driver; }

void* LoadSparkbox() {
  return host::LoadLibrary("../../lib/sparkbox/libsparkbox.so");
}

sparkbox::SparkboxInterface* CreateSparkbox(
    void* handle, sparkbox::CoreDriver& core_driver,
    sparkbox::filesystem::FilesystemDriver& fs_driver,
    sparkbox::audio::AudioDriver& audio_driver,
    sparkbox::controller::ControllerDriver& controller_driver) {
  using fn_type =
      sparkbox::SparkboxInterface* (*)(sparkbox::CoreDriver&,
                                       sparkbox::filesystem::FilesystemDriver&,
                                       sparkbox::audio::AudioDriver&,
                                       sparkbox::controller::ControllerDriver&);
  fn_type create_sparkbox;

  void* symbol = host::GetLibrarySymbol(handle, "CreateSparkbox");
  if (symbol == nullptr) return nullptr;

  create_sparkbox = reinterpret_cast<fn_type>(symbol);
  return create_sparkbox(core_driver, fs_driver, audio_driver,
                         controller_driver);
}

void UnloadSparkbox(void* handle) { host::UnloadLibrary(handle); }

}  // namespace device