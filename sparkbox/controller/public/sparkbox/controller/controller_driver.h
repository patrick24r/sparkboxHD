#pragma once

#include <functional>

#include "sparkbox/status.h"

namespace {

using ::sparkbox::Status;
} // namespace

namespace sparkbox::controller {

class ControllerDriver {
 public:
  using Callback = std::function<void()>;

  // Returns the number of supported controllers
  virtual int NumberofControllers() const;

  // Sets the callback for when new controller input is recieved
  // This includes connected/disconnected controllers
  virtual Status SetControllerCallback(Callback& callback);

  
  Status SetRumbleStatus(bool vibrating);


 protected:
};
    
} // namespace sparkbox::controller
