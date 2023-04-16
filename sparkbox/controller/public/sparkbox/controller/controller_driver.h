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
  using Callback = std::function<void()>;

  // Sets the callback for when new controller input is recieved.
  // This includes connected/disconnected controllers
  virtual Status SetOnControllerChanged(Callback& callback) = 0;
};
    
} // namespace sparkbox::controller
