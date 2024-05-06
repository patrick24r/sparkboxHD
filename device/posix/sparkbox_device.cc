#include "sparkbox_device.h"

#include "host_audio_driver.h"
#include "host_controller_driver.h"
#include "host_core_driver.h"
#include "host_filesystem_driver.h"
#include "sparkbox/sparkbox.h"

namespace {
using namespace ::sparkbox;
}

namespace device {

sparkbox::Sparkbox& GetSparkbox() {
  static shared::host::CoreDriver core_driver;
  static shared::host::HostFilesystemDriver fs_driver;
  static shared::host::HostControllerDriver cont_driver;
  static shared::host::HostAudioDriver audio_driver;
  static sparkbox::Sparkbox sbox =
      Sparkbox(core_driver, fs_driver, audio_driver, cont_driver);
  return sbox;
}

}  // namespace device