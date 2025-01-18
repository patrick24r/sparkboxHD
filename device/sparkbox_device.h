#pragma once

#include "device/app/application_driver.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/sparkbox_interface.h"

namespace device {

app::CoreAppDriver& GetCoreDriver();
app::FilesystemAppDriver& GetFilesystemDriver();
app::AudioAppDriver& GetAudioDriver();
app::ControllerAppDriver& GetControllerDriver();

void* LoadSparkbox();
sparkbox::SparkboxInterface* CreateSparkbox(
    void* handle, sparkbox::CoreDriver& core_driver,
    sparkbox::filesystem::FilesystemDriver& fs_driver,
    sparkbox::audio::AudioDriver& audio_driver,
    sparkbox::controller::ControllerDriver& controller_driver);
void UnloadSparkbox(void* handle);

}  // namespace device