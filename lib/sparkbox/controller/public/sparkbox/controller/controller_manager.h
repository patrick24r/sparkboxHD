#pragma once

#include <array>
#include <bitset>

#include "FreeRTOS.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_state.h"
#include "task.h"

namespace sparkbox::controller {

class ControllerManager {
 public:
  ControllerManager(ControllerDriver& driver)
      : driver_(driver), task_handle_(nullptr), new_controller_inputs_(0) {}

  sparkbox::Status SetUp(void);
  void TearDown(void);

  Status GetControllerState(int controller);

 private:
  ControllerDriver& driver_;
  std::array<ControllerState, ControllerDriver::kMaxControllers>
      controllers_state_;

  constexpr static size_t kTaskStackDepth = configMINIMAL_STACK_SIZE;
  constexpr static size_t kTaskPriority = 5;

  static_assert(kTaskStackDepth >= configMINIMAL_STACK_SIZE);

  TaskHandle_t task_handle_;
  static void ControllerTaskWrapper(void* controllerManager);
  void ControllerTask(void);
  Status OnInputChanged(int controllerIndex);

  std::bitset<ControllerDriver::kMaxControllers> new_controller_inputs_;
};

}  // namespace sparkbox::controller