#pragma once

#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/controller/controller_state.h"
#include "sparkbox/status.h"

namespace {

using ::sparkbox::controller::ControllerDriver;
using ::sparkbox::controller::ControllerState;
using ::sparkbox::Status;

} // namespace

namespace device::shared::host {

class HostControllerDriver : public ControllerDriver {
 public:
  Status SetOnInputChanged(Callback& callback) final;
  Status GetControllerState(int controllerIndex, ControllerState& state) final;

 private:
  Callback on_input_changed_cb_;
};

} // namespace device::shared::host