#pragma once

#include <functional>

#include "sparkbox/controller/controller_driver.h"

namespace sparkbox::controller {

class ControllerController {
 public:
  ControllerController(ControllerDriver& driver) : driver_(driver) {
    ControllerDriver::Callback cb = std::bind(&ControllerController::OnControllerChanged, this);
    driver_.SetOnControllerChanged(cb);
  }


 private:
  void OnControllerChanged() {}

  ControllerDriver& driver_;
};

} // namespace sparkbox::controller