#pragma once

#include <functional>

#include "stm32h7xx.h"

namespace nucleoh743zi2::core {

class InterruptManager final {
 public:
  static constexpr uint32_t kMaxInterrupts = 255; // Max IRQn_Type
  static constexpr uint32_t kMaxCallbacksPerInterrupt = 5;

  using Callback = std::function<void()>;
  static void RegisterCallback(IRQn_Type interrupt, Callback& callback);
  static InterruptManager& Instance();
 private:
  InterruptManager();
};

} // namepsace nucleoh743zi2::core