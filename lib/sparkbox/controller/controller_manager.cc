#include "sparkbox/controller/controller_manager.h"

#include <functional>

#include "sparkbox/assert.h"
#include "sparkbox/log.h"
#include "FreeRTOS.h"
#include "task.h"

namespace sparkbox::controller {

void ControllerManager::ControllerTaskWrapper(void * controller) {
  SP_ASSERT(controller != nullptr);
  ControllerManager * manager = static_cast<ControllerManager*>(controller);
  manager->ControllerTask();
}

Status ControllerManager::SetUp(void) {

  // Set up the controller manager task
  xTaskCreate(ControllerTaskWrapper,
              "ControllerTask",
              ControllerManager::TaskStackDepth,
              this,
              ControllerManager::TaskPriority,
              &this->task_handle_);

  // Configure the callback indicating controller state changed
  // ControllerDriver::Callback cb = std::bind(OnInputChanged, this);
  // driver_.SetOnInputChanged(cb);

  return Status::kOk;
}

void ControllerManager::TearDown(void) {
  // Tear down the controller manager task
}


void ControllerManager::OnInputChanged() {
  // Set flag that new input is detected

}

}