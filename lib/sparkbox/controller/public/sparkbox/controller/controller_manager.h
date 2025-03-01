#pragma once

#include <array>
#include <bitset>

#include "FreeRTOS.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_manager_interface.h"
#include "sparkbox/controller/controller_state.h"
#include "sparkbox/manager.h"
#include "sparkbox/message.h"
#include "sparkbox/router.h"
#include "sparkbox/status.h"

namespace sparkbox::controller {

class ControllerManager : public ControllerManagerInterface, sparkbox::Manager {
 public:
  ControllerManager(Router& router, ControllerDriver& driver)
      : sparkbox::Manager(router, Destination::kControllerManager),
        driver_(driver) {}

  sparkbox::Status SetUp(void) override;
  void TearDown(void) override;

  sparkbox::Status GetControllerState(int controller) final;

 private:
  ControllerDriver& driver_;
  std::array<ControllerState, ControllerDriver::kMaxControllers>
      controllers_state_;

  void HandleMessage(sparkbox::Message& message) override;
  sparkbox::Status InputChangedCb(int controllerIndex);
};

}  // namespace sparkbox::controller