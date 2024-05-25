#pragma once

#include <cstdint>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sparkbox/status.h"

namespace sparkbox {

class Task {
 public:
  static constexpr size_t kDefaultStackDepth = configMINIMAL_STACK_SIZE;
  static constexpr uint32_t kTaskPriority = 5;
  Task()
      : name_(nullptr),
        function_(nullptr),
        stack_depth_(kDefaultStackDepth),
        task_handle_(nullptr) {}
  Task(const char* name, TaskFunction_t function,
       size_t stack_depth = kDefaultStackDepth)
      : name_(name),
        function_(function),
        stack_depth_(stack_depth),
        task_handle_(nullptr) {}

  // Add task to the scheduler with the passed data
  void AddToScheduler(void* data);
  void RemoveFromScheduler();

 protected:
  const char* name_;
  const TaskFunction_t function_;
  const size_t stack_depth_;
  TaskHandle_t task_handle_;
};

}  // namespace sparkbox