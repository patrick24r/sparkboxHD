#pragma once

#include <array>
#include <functional>

#include "sparkbox/controller/controller_state.h"
#include "sparkbox/status.h"

namespace {
using ::sparkbox::Status;
} // namespace

namespace sparkbox::controller {

class ControllerDriver {
 public:
  static constexpr int kMaxControllers = 4;
  using Callback = std::function<Status(int)>;

  virtual Status SetUp(void) = 0;
  virtual void TearDown(void) = 0;

  // Sets the callback for when new controller input is recieved.
  // This includes connected/disconnected controllers
  virtual Status SetOnInputChanged(Callback& callback) = 0;

  // Get the state of the given controller
  virtual Status GetControllerState(int controllerIndex, ControllerState& state) = 0;
};
    
} // namespace sparkbox::controller
