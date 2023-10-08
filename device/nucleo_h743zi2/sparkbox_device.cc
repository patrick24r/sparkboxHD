#include "sparkbox_device.h"

#include "nucleo_h743zi2/controller_driver.h"
#include "nucleo_h743zi2/filesystem_driver.h"
#include "sparkbox/sparkbox.h"

namespace device {

sparkbox::Sparkbox& GetSparkbox() {
  static NucleoH743ZI2::FilesystemDriver fs_driver;
  static NucleoH743ZI2::ControllerDriver cont_driver;
  static sparkbox::Sparkbox sbox = sparkbox::Sparkbox(fs_driver, cont_driver);
  return sbox;
}

} // namespace device