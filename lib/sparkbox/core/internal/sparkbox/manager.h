#pragma once

#include <cstdint>
#include <string>

#include "FreeRTOS.h"
#include "queue.h"
#include "sparkbox/assert.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"
#include "task.h"

namespace sparkbox {

class Manager {
 public:
  struct Config {
    const char* task_name;
    size_t task_stack_depth;
    size_t task_priority;
    size_t queue_length;
  };
  Manager(const Config& config) : config_(config) {
    SP_ASSERT(config.task_stack_depth >= configMINIMAL_STACK_SIZE);
  }

  virtual Status SetUp(void);
  virtual void TearDown(void);

 protected:
  const Config& config_;
  // Handle the message on the manager's task
  virtual void HandleMessage(Message& message) = 0;

  void SendInternalMessage(Message& message);
  void SendInternalMessageISR(Message& message);

 private:
  TaskHandle_t task_handle_;
  QueueHandle_t queue_handle_;

  static void TaskWrapper(void* manager_ptr);
  void TaskFunction(void);
};

}  // namespace sparkbox