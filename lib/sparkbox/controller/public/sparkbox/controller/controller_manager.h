#pragma once

#include <array>

#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_state.h"
#include "FreeRTOS.h"
#include "task.h"

namespace {

using ::sparkbox::controller::ControllerDriver;
using ::sparkbox::controller::ControllerState;

} // namespace

namespace sparkbox::controller {

class ControllerManager {
 public:
  ControllerManager(ControllerDriver& driver) : driver_(driver) {}

  Status SetUp(void);
  void TearDown(void);

  Status GetControllerState(int controller);

 private:
  ControllerDriver& driver_;
  std::array<ControllerState, ControllerDriver::kMaxControllers> controllers_state_;


  constexpr static size_t TaskStackDepth = 128;
  constexpr static size_t TaskPriority = 16;
  TaskHandle_t task_handle_;
  static void ControllerTaskWrapper(void * controller);
  void ControllerTask(void) { while(1); }
  void OnInputChanged(void);

  
};

} // namespace sparkbox::controller