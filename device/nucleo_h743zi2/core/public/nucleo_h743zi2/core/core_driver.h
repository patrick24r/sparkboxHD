#pragma once

#include <array>
#include <cstdint>

#include "nucleo_h743zi2/core/clock.h"
#include "nucleo_h743zi2/core/pin.h"
#include "sparkbox/core_driver.h"
#include "sparkbox/status.h"

namespace nucleoh743zi2::core {

class CoreDriver final : public sparkbox::CoreDriver {
 public:
  sparkbox::Status SetUp(void) final;
  void TearDown(void) final;
 private:
  Clock clock_;
  std::array<OutputPin, 3> leds_ = {
    OutputPin(GPIOB, 0U), // LED0 - PB0
    OutputPin(GPIOE, 1U), // LED1 - PE1
    OutputPin(GPIOB, 14U) // LED2 - PB14
  };
};

} // namespace nucleoh743zi2