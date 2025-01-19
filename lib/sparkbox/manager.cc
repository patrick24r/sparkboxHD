#include "sparkbox/manager.h"

#include "freertos/queue.h"
#include "freertos/task.h"
#include "sparkbox/assert.h"
#include "sparkbox/log.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"

namespace {
using sparkbox::Status;
}  // namespace

namespace sparkbox {

Status Manager::SetUp(void) {
  // Create the manager's message queue for sparkbox messages
  queue_handle_ = xQueueCreate(queue_depth_, sizeof(sparkbox::Message));
  SP_ASSERT(queue_handle_ != nullptr);

  // Let the router know that this queue exists at our destination
  router_.RegisterQueue(destination_, queue_handle_);

  // Create the manager's task and add it to the scheduler. Pass in a pointer to
  // "this" so we can go from static TaskWrapper to non-static TaskFunction
  task_.AddToScheduler(this);

  return Status::kOk;
}

Status Manager::SendMessage(Destination destination, Message& message) {
  Message message_to_send =
      Message(message.type(), AllocateAndCopyMessageFrom(message),
              message.size_bytes());
  return router_.SendMessage(destination, message_to_send);
}

Status Manager::SendMessageISR(Destination destination, Message& message) {
  // Copy the message and its data to a new message we will keep allocated until
  // the message is handled by the receiver
  Message message_to_send =
      Message(message.type(), AllocateAndCopyMessageFrom(message),
              message.size_bytes());
  return router_.SendMessageISR(destination, message_to_send);
}

uint8_t* Manager::AllocateAndCopyMessageFrom(Message& message) {
  // Allocate new memory to persist until the message is handled, then copy
  uint8_t* persistent_message_data = new uint8_t[message.size_bytes()];
  std::memcpy(persistent_message_data, message.payload_ptr_as<uint8_t>(),
              message.size_bytes());
  return persistent_message_data;
}

void Manager::TaskWrapper(void* manager_ptr) {
  SP_ASSERT(manager_ptr != nullptr);
  static_cast<Manager*>(manager_ptr)->TaskFunction();
}

void Manager::TaskFunction() {
  // Wait for messages in the queue and send them to dispatch
  while (1) {
    // While the queue is not empty, dispatch the messages
    while (!xQueueIsQueueEmptyFromISR(queue_handle_)) {
      Message message;
      SP_ASSERT(xQueueReceive(queue_handle_, &message, 0) == pdTRUE);
      HandleMessage(message);

      // Now that the message has been handled, delete the allocated memory
      delete message.payload_ptr_as<uint8_t>();
    }
    taskYIELD();
  }
}

void Manager::TearDown(void) {
  task_.RemoveFromScheduler();

  router_.UnregisterQueue(destination_);

  if (queue_handle_ != nullptr) {
    vQueueDelete(queue_handle_);
  }
}

}  // namespace sparkbox