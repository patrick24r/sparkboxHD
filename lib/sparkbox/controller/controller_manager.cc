#include "sparkbox/controller/controller_manager.h"

#include <bitset>
#include <functional>

#include "sparkbox/assert.h"
#include "sparkbox/log.h"
#include "FreeRTOS.h"
#include "task.h"

namespace {

using std::placeholders::_1;

}

namespace sparkbox::controller {

Status ControllerManager::SetUp(void) {
  // Set up the controller driver
  Status driver_ret = driver_.SetUp();
  SP_ASSERT(driver_ret == Status::kOk);

  // Set up the controller manager task
  BaseType_t task_ret = xTaskCreate(ControllerTaskWrapper,
                                    "ControllerTask",
                                    ControllerManager::kTaskStackDepth,
                                    this,
                                    ControllerManager::kTaskPriority,
                                    &task_handle_);
  SP_ASSERT(task_ret == pdPASS);

  // Configure the callback indicating controller state changed
  ControllerDriver::Callback cb = std::bind(&ControllerManager::OnInputChanged, this, _1);
  driver_ret = driver_.SetOnInputChanged(cb);
  SP_ASSERT(driver_ret == Status::kOk);

  return Status::kOk;
}

void ControllerManager::TearDown(void) {
  // Tear down the controller manager task
  if (task_handle_ != nullptr) {
    vTaskDelete(task_handle_);
  }

  driver_.TearDown();
}

void ControllerManager::ControllerTaskWrapper(void * controllerManager) {
  SP_ASSERT(controllerManager != nullptr);
  ControllerManager * manager = static_cast<ControllerManager*>(controllerManager);
  manager->ControllerTask();
}

void ControllerManager::ControllerTask(void) {
  Status driver_status = Status::kOk;

  while (1) {
    // If no controllers have any new input, do nothing
    if (new_controller_inputs_.any()) {
      for (uint cont_idx = 0; cont_idx < controllers_state_.size(); cont_idx++) {
        // Skip controllers that have no new input
        if (!new_controller_inputs_[cont_idx]) {
          continue;
        }

        // Get the latest state of the controller with new inputs
        driver_status = driver_.GetControllerState(cont_idx, controllers_state_[cont_idx]);
        if (driver_status == Status::kOk) {
          new_controller_inputs_[cont_idx] = false;
        } else if (driver_status == Status::kUnavailable) {
          SP_LOG_ERROR("Controller driver unavailable, retrying at a later time");
        } else {
          SP_LOG_ERROR("Unrecoverable error getting controller state: %d", static_cast<int>(driver_status));
          SP_ASSERT(false);
        }
      }
    }
    

    taskYIELD();
  }
}

Status ControllerManager::OnInputChanged(int controllerIndex) {
  if (controllerIndex >= static_cast<int>(new_controller_inputs_.size()) ||
      controllerIndex < 0) {
    return Status::kBadParameter;
  }

  // Set flag that a new input is detected for the given controller
  new_controller_inputs_[controllerIndex] = true;
  return Status::kOk;
}

}