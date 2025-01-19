#include "sparkbox/router.h"

#include "FreeRTOS.h"
#include "queue.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"

namespace sparkbox {

Status Router::RegisterQueue(Destination destination, QueueHandle_t queue) {
  if (KeyInQueueMap(destination)) return Status::kAlreadyExists;
  queues_[static_cast<int>(destination)] = queue;
  return Status::kOk;
}

Status Router::UnregisterQueue(Destination destination) {
  if (!KeyInQueueMap(destination)) return Status::kBadResourceState;
  queues_[static_cast<int>(destination)] = nullptr;
  return Status::kOk;
}

Status Router::SendMessage(Destination destination, Message& message) {
  if (!KeyInQueueMap(destination)) return Status::kBadResourceState;

  BaseType_t ret =
      xQueueSend(queues_[static_cast<int>(destination)], &message, 0);
  if (ret != pdTRUE) {
    SP_LOG_ERROR("Error sending queue message: %ld", static_cast<long>(ret));
    return Status::kUnknown;
  }

  return Status::kOk;
}

Status Router::SendMessageISR(Destination destination, Message& message) {
  if (!KeyInQueueMap(destination)) return Status::kBadResourceState;

  BaseType_t unused;
  BaseType_t ret = xQueueSendFromISR(queues_[static_cast<int>(destination)],
                                     &message, &unused);
  if (ret != pdTRUE) {
    SP_LOG_ERROR("Error sending queue message: %ld", static_cast<long>(ret));
    return Status::kUnknown;
  }

  return Status::kOk;
}

bool Router::KeyInQueueMap(Destination key) {
  return (queues_[static_cast<int>(key)] != nullptr);
}

}  // namespace sparkbox