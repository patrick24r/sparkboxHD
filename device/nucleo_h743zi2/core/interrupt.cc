#include "nucleo_h743zi2/core/interrupt.h"

#include <array>

#include "stm32h7xx.h"


#define IT_CB(x) if(interrupt_callbacks[x] != nullptr) (*interrupt_callbacks[x])();

namespace nucleoh743zi2::core {

static std::array<InterruptManager::Callback*,
                  InterruptManager::kMaxInterrupts> interrupt_callbacks;

extern "C" {

extern void SysTick_Handler(void);
extern void TIM7_IRQHandler(void);

void Systick_Handler(void) {
  IT_CB(SysTick_IRQn);
}

void TIM7_IRQHandler(void) {
  IT_CB(TIM7_IRQn);
}

} // extern "C"

InterruptManager& InterruptManager::Instance() {
  static InterruptManager it_mgr;
  return it_mgr;
}

void InterruptManager::RegisterCallback(IRQn_Type interrupt,
                                        Callback& callback) {
  interrupt_callbacks[interrupt] = &callback;
}

} // namespace nucleoh743zi2::core