#pragma once

#include "device/app/application_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_state.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace device::host {

class HostControllerDriver : public device::app::ControllerAppDriver {
 public:
  using Callback = sparkbox::controller::ControllerDriver::Callback;
  void SetUp() final { SP_LOG_DEBUG("Host controller driver set up..."); }
  void TearDown() final { SP_LOG_DEBUG("Host controller driver tear down..."); }

  sparkbox::Status SetOnInputChanged(Callback& callback) final;
  sparkbox::Status GetControllerState(
      int controllerIndex, sparkbox::controller::ControllerState& state) final;

 private:
  Callback on_input_changed_cb_ = NULL;
};

}  // namespace device::host