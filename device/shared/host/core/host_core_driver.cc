#include "host_core_driver.h"

#include "sparkbox/status.h"

namespace device::shared::host {

sparkbox::Status CoreDriver::SetUp(void) {
  return sparkbox::Status::kOk;
}

void CoreDriver::TearDown(void) {}

} // namespace device::shared::host