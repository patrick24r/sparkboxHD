#pragma once

#include "sparkbox/controller/controller_driver.h"

namespace {
using ::sparkbox::controller::ControllerDriver;
using ::sparkbox::Status;
} // namespace

namespace device::shared::host {

class HostControllerDriver : public ControllerDriver {
 public:
  Status SetOnControllerChanged(ControllerDriver::Callback& callback) override;

 private:
  ControllerDriver::Callback callback;
};

} // namespace device::shared::host