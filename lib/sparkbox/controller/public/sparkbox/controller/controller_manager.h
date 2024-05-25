#pragma once

#include <array>
#include <bitset>

#include "FreeRTOS.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_state.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/status.h"

namespace sparkbox::controller {

class ControllerManager : sparkbox::Manager {
 public:
  ControllerManager(ControllerDriver& driver)
      : sparkbox::Manager("ControllerTask"), driver_(driver) {}

  sparkbox::Status SetUp(void) override;
  void TearDown(void) override;

  Status GetControllerState(int controller);

 private:
  ControllerDriver& driver_;
  std::array<ControllerState, ControllerDriver::kMaxControllers>
      controllers_state_;

  void HandleMessage(sparkbox::Message& message) override;
  sparkbox::Status InputChangedCb(int controllerIndex);
};

}  // namespace sparkbox::controller