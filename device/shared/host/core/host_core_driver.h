#pragma once

#include "sparkbox/core_driver.h"
#include "sparkbox/status.h"

namespace device::shared::host {

class CoreDriver final : public sparkbox::CoreDriver {
 public:
  sparkbox::Status SetUp(void) final;
  void TearDown(void) final;
};

} // namespace device::shared::host