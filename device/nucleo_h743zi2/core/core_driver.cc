#include "nucleo_h743zi2/core/core_driver.h"

#include "nucleo_h743zi2/core/clock.h"
#include "nucleo_h743zi2/core/pin.h"
#include "sparkbox/assert.h"

namespace nucleoh743zi2::core {

sparkbox::Status CoreDriver::SetUp(void) {
  HAL_Init();
  clock_.SetUp();
  for (OutputPin led : leds_) {
    led.SetUp();
  }

  // 
  
  return sparkbox::Status::kOk;
}

void CoreDriver::TearDown(void) {
  return;
}

} // namespace nucleoh743zi2::core