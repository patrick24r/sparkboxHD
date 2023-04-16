#include "host_controller_driver.h"

namespace {
using ::sparkbox::controller::ControllerDriver;
using ::sparkbox::Status;
} // namespace

namespace device::shared::host {

Status HostControllerDriver::SetOnControllerChanged(Callback& callback) {
    return Status::kOk;
}

} // namespace device::shared::host