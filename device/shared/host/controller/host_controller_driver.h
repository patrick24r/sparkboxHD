#pragma once

#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_state.h"
#include "sparkbox/status.h"

namespace device::shared::host {

class HostControllerDriver : public sparkbox::controller::ControllerDriver {
 public:
  sparkbox::Status SetUp() final;
  void TearDown() final;

  sparkbox::Status SetOnInputChanged(Callback& callback) final;
  sparkbox::Status GetControllerState(
      int controllerIndex, sparkbox::controller::ControllerState& state) final;

 private:
  Callback on_input_changed_cb_ = NULL;
};

}  // namespace device::shared::host