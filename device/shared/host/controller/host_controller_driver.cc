#include "host_controller_driver.h"

namespace {
using ::sparkbox::Status;
using ::sparkbox::controller::ControllerDriver;
using ::sparkbox::controller::ControllerState;
}  // namespace

namespace device::shared::host {

Status HostControllerDriver::SetUp(void) { return Status::kOk; }

void HostControllerDriver::TearDown(void) {}

Status HostControllerDriver::SetOnInputChanged(Callback& callback) {
  // Save the callback to call when the input changes
  on_input_changed_cb_ = callback;
  return Status::kOk;
}

Status HostControllerDriver::GetControllerState(int controllerIndex,
                                                ControllerState& state) {
  return Status::kOk;
}

}  // namespace device::shared::host