#include "sparkbox/task.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sparkbox/assert.h"
#include "sparkbox/log.h"

namespace sparkbox {

// Adding a new task. Create the task and a queue for it to receive messages in
void Task::AddToScheduler(void* data) {
  // If the task already exists and was added, do nothing
  if (task_handle_ != nullptr) {
    SP_LOG_WARN("Task has already been added to the scheduler");
    return;
  }

  SP_ASSERT(stack_depth_ >= configMINIMAL_STACK_SIZE);
  SP_ASSERT(xTaskCreate(function_, name_, stack_depth_, data, kTaskPriority,
                        &task_handle_) == pdPASS);
}

void Task::RemoveFromScheduler() {
  SP_ASSERT(task_handle_ != nullptr);
  vTaskDelete(task_handle_);
  task_handle_ = nullptr;
}

}  // namespace sparkbox