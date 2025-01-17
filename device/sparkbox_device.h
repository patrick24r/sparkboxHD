#pragma once

#include "device/app/application_driver.h"
#include "sparkbox/sparkbox.h"

namespace device {

app::CoreAppDriver& GetCoreDriver();
app::FilesystemAppDriver& GetFilesystemDriver();
app::AudioAppDriver& GetAudioDriver();
app::ControllerAppDriver& GetControllerDriver();

}  // namespace device