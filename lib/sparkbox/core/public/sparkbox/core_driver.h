#pragma once

#include "sparkbox/status.h"

namespace sparkbox {

class CoreDriver {
 public:
  virtual Status SetUp(void) = 0;
  virtual void TearDown(void) = 0;
};

} // namespace sparkbox