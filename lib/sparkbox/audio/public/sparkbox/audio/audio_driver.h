#pragma once

#include "sparkbox/status.h"
namespace sparkbox::audio {

class AudioDriver {
 public:
  virtual sparkbox::Status SetUp(void) = 0;
  virtual sparkbox::Status TearDown(void) = 0;
};

}  // namespace sparkbox::audio