#pragma once

#include "sparkbox/status.h"

namespace sparkbox::controller {

class ControllerManagerInterface {
 public:
  virtual sparkbox::Status GetControllerState(int controller) = 0;
};

}  // namespace sparkbox::controller