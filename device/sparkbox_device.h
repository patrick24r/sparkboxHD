#pragma once

#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"

namespace device {

sparkbox::CoreDriver& GetCoreDriver();
sparkbox::filesystem::FilesystemDriver& GetFilesystemDriver();
sparkbox::audio::AudioDriver& GetAudioDriver();
sparkbox::controller::ControllerDriver& GetControllerDriver();

}  // namespace device