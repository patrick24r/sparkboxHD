#pragma once

#include <array>

#include "FreeRTOS.h"
#include "queue.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"

namespace sparkbox {

enum class Destination : int {
  kCoreManager = 0,
  kControllerManager,
  kAudioManager,
  kVideoManager,
  // Max destinations. Do not add an item after this
  kMaxDestinations,
};

class Router {
 public:
  Status RegisterQueue(Destination destination, QueueHandle_t queue);
  Status UnregisterQueue(Destination destination);

  Status SendMessage(Destination destination, Message& message);
  Status SendMessageISR(Destination destination, Message& message);

 private:
  std::array<QueueHandle_t, static_cast<int>(Destination::kMaxDestinations)>
      queues_;

  bool KeyInQueueMap(Destination key);
};

}  // namespace sparkbox