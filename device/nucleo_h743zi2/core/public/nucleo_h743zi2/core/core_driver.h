#pragma once

#include "sparkbox/core_driver.h"
#include "sparkbox/status.h"

namespace NucleoH743ZI2 {

class CoreDriver final : public sparkbox::CoreDriver {
 public:
  sparkbox::Status SetUp(void) final;
  void TearDown(void) final;
};

} // namespace NucleoH743ZI2