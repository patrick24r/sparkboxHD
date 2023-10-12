#include "nucleo_h743zi2/core/core_driver.h"

namespace NucleoH743ZI2 {

sparkbox::Status CoreDriver::SetUp(void) {
  return sparkbox::Status::kOk;
}

void CoreDriver::TearDown(void) {
  return;
}

} // namespace NucleoH743ZI2