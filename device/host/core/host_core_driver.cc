#include "host_core_driver.h"

#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace device::host {

void CoreDriver::SetUp(void) { SP_LOG_DEBUG("Host core driver set up..."); }

void CoreDriver::TearDown(void) {
  SP_LOG_DEBUG("Host core driver tear down...");
}

}  // namespace device::host