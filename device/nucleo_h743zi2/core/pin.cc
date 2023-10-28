#include "nucleo_h743zi2/core/pin.h"

#include "sparkbox/assert.h"
#include "stm32h7xx.h"
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_gpio.h"

namespace nucleoh743zi2::core {

/****************************** Pin *****************************/

void Pin::SetUp(PinConfig& config) {
  if (port_ == GPIOA) {
    __HAL_RCC_GPIOA_CLK_ENABLE();
  } else if (port_ == GPIOB) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
  } else if (port_ == GPIOC) {
    __HAL_RCC_GPIOC_CLK_ENABLE();
  } else if (port_ == GPIOD) {
    __HAL_RCC_GPIOD_CLK_ENABLE();
  } else if (port_ == GPIOE) {
    __HAL_RCC_GPIOE_CLK_ENABLE();
  } else if (port_ == GPIOF) {
    __HAL_RCC_GPIOF_CLK_ENABLE();
  } else if (port_ == GPIOG) {
    __HAL_RCC_GPIOG_CLK_ENABLE();
  } else if (port_ == GPIOH) {
    __HAL_RCC_GPIOH_CLK_ENABLE();
  } else {
    SP_ASSERT(false);
  }

  HAL_GPIO_Init(port_, &config);
}

void Pin::TearDown() {

}

/****************************** OutputPin *****************************/
void OutputPin::SetUp() {
  PinConfig init_config = {
    .Pin = pin_mask_,
    .Mode = GPIO_MODE_OUTPUT_PP, // Output push-pull
    .Pull = GPIO_NOPULL, // No pull-up/pull-down
    .Speed = GPIO_SPEED_FREQ_LOW, // low frequency
  };

  Pin::SetUp(init_config);
}

void OutputPin::Set() {
  HAL_GPIO_WritePin(port_, pin_mask_, GPIO_PIN_SET);
}

void OutputPin::Reset() {
  HAL_GPIO_WritePin(port_, pin_mask_, GPIO_PIN_RESET);
}

void OutputPin::Toggle() {
  HAL_GPIO_TogglePin(port_, pin_mask_);
}

} // namepsace nucleoh743zi2::core