#pragma once

#include <cstdint>
#include <string>

#include "FreeRTOS.h"
#include "queue.h"
#include "sparkbox/assert.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"
#include "sparkbox/task.h"

namespace sparkbox {

// A manager is a task with a built in queue and function wrapper for receiving
// a queue of messages guaranteed to be in its own task context
class Manager {
 public:
  static constexpr size_t kDefaultQueueDepth = 20;
  Manager(const char* task_name,
          size_t task_stack_depth = Task::kDefaultStackDepth,
          size_t queue_depth = kDefaultQueueDepth)
      : task_(task_name, TaskWrapper, task_stack_depth),
        queue_depth_(queue_depth),
        queue_handle_(nullptr) {}

  virtual Status SetUp(void);
  virtual void TearDown(void);

 protected:
  // Handle the message on the manager's task
  virtual void HandleMessage(Message& message) = 0;

  void SendInternalMessage(Message& message);
  void SendInternalMessageISR(Message& message);

 private:
  Task task_;
  const size_t queue_depth_;
  QueueHandle_t queue_handle_;

  static void TaskWrapper(void* manager_ptr);
  void TaskFunction(void);
};

}  // namespace sparkbox