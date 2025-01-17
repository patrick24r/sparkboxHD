#include "sparkbox_device.h"

#include "device/app/application_driver.h"
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

app::CoreAppDriver& GetCoreDriver() { return core_driver; }
app::FilesystemAppDriver& GetFilesystemDriver() { return filesystem_driver; }
app::AudioAppDriver& GetAudioDriver() { return audio_driver; }
app::ControllerAppDriver& GetControllerDriver() { return controller_driver; }

}  // namespace device