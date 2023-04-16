#include "sparkbox_device.h"
#include "host_controller_driver.h"
#include "host_filesystem_driver.h"
#include "sparkbox/sparkbox.h"

namespace {
using namespace ::sparkbox;
}

namespace device {

sparkbox::Sparkbox& GetSparkbox() {
  static shared::host::HostFilesystemDriver fs_driver;
  static shared::host::HostControllerDriver cont_driver;
  static sparkbox::Sparkbox sbox = Sparkbox(fs_driver, cont_driver);
  return sbox;
}

} // namespace device