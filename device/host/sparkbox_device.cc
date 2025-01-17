#include "sparkbox_device.h"

#include "host_audio_driver.h"
#include "host_controller_driver.h"
#include "host_core_driver.h"
#include "host_filesystem_driver.h"
#include "sparkbox/sparkbox.h"

namespace device {

// Getters for static device drivers
host::CoreDriver core_driver;
host::HostFilesystemDriver filesystem_driver;
host::HostAudioDriver audio_driver;
host::HostControllerDriver controller_driver;

sparkbox::CoreDriver& GetCoreDriver() { return core_driver; }
sparkbox::filesystem::FilesystemDriver& GetFilesystemDriver() {
  return filesystem_driver;
}
sparkbox::audio::AudioDriver& GetAudioDriver() { return audio_driver; }
sparkbox::controller::ControllerDriver& GetControllerDriver() {
  return controller_driver;
}

}  // namespace device