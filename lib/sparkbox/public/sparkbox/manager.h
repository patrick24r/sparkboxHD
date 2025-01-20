#pragma once

#include <cstdint>
#include <string>

#include "FreeRTOS.h"
#include "queue.h"
#include "sparkbox/assert.h"
#include "sparkbox/message.h"
#include "sparkbox/router.h"
#include "sparkbox/status.h"
#include "sparkbox/task.h"

#define SP_STRINGIZE_ENUM(enum_value) #enum_value

namespace sparkbox {

// A manager is a task with a built in queue and function wrapper for receiving
// a queue of messages guaranteed to be in its own task context
class Manager {
 public:
  static constexpr size_t kDefaultQueueDepth = 20;
  Manager(Router& router, Destination destination,
          size_t task_stack_depth = Task::kDefaultStackDepth,
          size_t queue_depth = kDefaultQueueDepth)
      : router_(router),
        destination_(destination),
        task_(SP_STRINGIZE_ENUM(destination), TaskWrapper, task_stack_depth),
        queue_depth_(queue_depth),
        queue_handle_(nullptr) {}
  virtual ~Manager() = default;

  virtual Status SetUp(void);
  virtual void TearDown(void);

 protected:
  // Handle the message on the manager's task
  virtual void HandleMessage(Message& message) = 0;

  Status SendMessage(Destination destination, Message& message);
  Status SendMessageISR(Destination destination, Message& message);

  Status SendInternalMessage(Message& message) {
    return SendMessage(destination_, message);
  }
  Status SendInternalMessageISR(Message& message) {
    return SendMessageISR(destination_, message);
  }

 private:
  Router& router_;
  const Destination destination_;
  Task task_;
  const size_t queue_depth_;
  QueueHandle_t queue_handle_;

  static void TaskWrapper(void* manager_ptr);
  void TaskFunction(void);

  uint8_t* AllocateAndCopyMessageFrom(Message& message);
};

}  // namespace sparkbox