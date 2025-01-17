#include "host_core_driver.h"

#include "sparkbox/status.h"

namespace device::host {

sparkbox::Status CoreDriver::SetUp(void) { return sparkbox::Status::kOk; }

void CoreDriver::TearDown(void) {}

}  // namespace device::host