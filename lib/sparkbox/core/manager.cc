#include "sparkbox/manager.h"

#include "queue.h"
#include "sparkbox/assert.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"
#include "task.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace sparkbox {

Status Manager::SetUp(void) {
  // Create the manager's task
  SP_ASSERT(xTaskCreate(TaskWrapper, config_.task_name,
                        config_.task_stack_depth, this, config_.task_priority,
                        &task_handle_) == pdPASS);

  // Create the manager's message queue for sparkbox messages
  queue_handle_ = xQueueCreate(config_.queue_length, sizeof(sparkbox::Message));
  SP_ASSERT(queue_handle_ != nullptr);

  return Status::kOk;
}

void Manager::SendInternalMessage(Message& message) {
  SP_ASSERT(queue_handle_ != nullptr);
  SP_ASSERT(xQueueSend(queue_handle_, &message, 0) == pdTRUE);
}
void Manager::SendInternalMessageISR(Message& message) {
  SP_ASSERT(queue_handle_ != nullptr);
  BaseType_t unused;
  SP_ASSERT(xQueueSendFromISR(queue_handle_, &message, &unused) == pdTRUE);
}

void Manager::TaskWrapper(void* manager_ptr) {
  SP_ASSERT(manager_ptr != nullptr);
  static_cast<Manager*>(manager_ptr)->TaskFunction();
}

void Manager::TaskFunction() {
  // Wait for messages in the queue and send them to dispatch
  while (1) {
    // While the queue is not empty, dispatch the messages
    while (queue_handle_ && !xQueueIsQueueEmptyFromISR(queue_handle_)) {
      Message message;
      SP_ASSERT(xQueueReceive(queue_handle_, &message, 0) == pdTRUE);
      HandleMessage(message);
    }
    taskYIELD();
  }
}

void Manager::TearDown(void) {
  if (task_handle_ != nullptr) {
    vTaskDelete(task_handle_);
  }

  if (queue_handle_ != nullptr) {
    vQueueDelete(queue_handle_);
  }
}

}  // namespace sparkbox