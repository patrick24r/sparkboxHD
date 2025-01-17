#pragma once

#include "device/app/application_driver.h"

namespace device::host {

class CoreDriver final : public device::app::CoreAppDriver {
 public:
  void SetUp() final;
  void TearDown() final;
};

}  // namespace device::host