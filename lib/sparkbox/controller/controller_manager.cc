#include "sparkbox/controller/controller_manager.h"

#include <bitset>
#include <functional>

#include "FreeRTOS.h"
#include "sparkbox/assert.h"
#include "sparkbox/log.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"
#include "task.h"

namespace {
using sparkbox::Message;
using sparkbox::MessageType;
using sparkbox::Status;
using std::placeholders::_1;
}  // namespace

namespace sparkbox::controller {

Status ControllerManager::SetUp(void) {
  // Set up the controller driver
  SP_ASSERT(Manager::SetUp() == Status::kOk);

  // Configure the callback indicating controller state changed
  ControllerDriver::Callback cb =
      std::bind(&ControllerManager::InputChangedCb, this, _1);
  SP_ASSERT(driver_.SetOnInputChanged(cb) == Status::kOk);

  return Status::kOk;
}

void ControllerManager::TearDown(void) {
  // Tear down in reverse order
  Manager::TearDown();
}

sparkbox::Status ControllerManager::GetControllerState(int controller) {
  return sparkbox::Status::kOk;
}

void ControllerManager::HandleMessage(Message &message) {
  if (message.message_type == MessageType::kControllerInputChanged) {
    // Update the data for the controller whose input changed
    int controller_index = *message.payload_as<int>();
    Status driver_status = driver_.GetControllerState(
        controller_index, controllers_state_[controller_index]);
    if (driver_status != Status::kOk) {
      SP_LOG_ERROR("Error getting controller state: %u",
                   static_cast<uint>(driver_status));
    }
  } else {
    SP_LOG_INFO("Unknown message type received by controller manager: %zu",
                static_cast<size_t>(message.message_type));
  }
}

Status ControllerManager::InputChangedCb(int controllerIndex) {
  Message message = Message(MessageType::kControllerInputChanged,
                            &controllerIndex, sizeof(int));
  SendInternalMessageISR(message);
  return Status::kOk;
}

}  // namespace sparkbox::controller