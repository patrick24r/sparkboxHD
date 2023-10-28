#pragma once

#include <cstdint>

#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"

namespace nucleoh743zi2::core {

class Pin {
 public:
  Pin(GPIO_TypeDef* port, uint16_t pin) 
    : port_(port),
      pin_mask_(1U << pin) {}

  using PinConfig = GPIO_InitTypeDef;
  void SetUp(PinConfig& config);
  virtual void TearDown();
 protected:
  GPIO_TypeDef* port_;
  uint32_t pin_mask_;
};

class OutputPin : public Pin {
 public:
  OutputPin(GPIO_TypeDef* port, uint16_t pin) : Pin(port, pin) {}
  void SetUp();

  void Set();
  void Reset();
  void Toggle();
};

} // namepsace nucleoh743zi2::core