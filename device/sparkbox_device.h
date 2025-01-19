#pragma once

#include "device/app/application_driver.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/sparkbox_interface.h"

namespace device {

app::CoreAppDriver& GetCoreDriver();
app::FilesystemAppDriver& GetFilesystemDriver();
app::AudioAppDriver& GetAudioDriver();
app::ControllerAppDriver& GetControllerDriver();

}  // namespace device