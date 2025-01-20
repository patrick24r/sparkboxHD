#pragma once

#include "sparkbox/core/core_driver.h"
#include "sparkbox/sparkbox_interface.h"

namespace sparkbox {

class CoreManager {
 public:
  CoreManager(SparkboxLevelInterface& sparkbox, CoreDriver& core_driver)
      : sparkbox_(sparkbox), core_driver_(core_driver) {}

  const char* LoadAndRunLevel(const char* level_name);

 private:
  SparkboxLevelInterface& sparkbox_;
  CoreDriver& core_driver_;
};

}  // namespace sparkbox