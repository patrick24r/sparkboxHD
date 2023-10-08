#pragma once

#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/status.h"

namespace NucleoH743ZI2 {

class ControllerDriver final : public sparkbox::controller::ControllerDriver {
 public:
  sparkbox::Status SetUp(void) final { return sparkbox::Status::kOk; }
  void TearDown(void) final {}

  sparkbox::Status SetOnInputChanged(Callback& callback) final { return sparkbox::Status::kOk; }

  sparkbox::Status GetControllerState(int controllerIndex, ControllerState& state) final { return sparkbox::Status::kOk; };
};

}