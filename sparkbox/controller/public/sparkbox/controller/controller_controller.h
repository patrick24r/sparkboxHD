#pragma once

#include "sparkbox/controller/controller_driver.h"

namespace sparkbox::controller {

class ControllerController {
 public:
  ControllerController(ControllerDriver& driver) : driver_(driver) {}


 private:
  ControllerDriver& driver_;
};

} // namespace sparkbox::controller