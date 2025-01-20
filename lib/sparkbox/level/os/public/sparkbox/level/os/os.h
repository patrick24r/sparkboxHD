#pragma once

#include "sparkbox/level/level.h"
#include "sparkbox/level/level_interface.h"
#include "sparkbox/sparkbox_interface.h"

namespace sparkbox::level::os {

class Os : public LevelInterface {
 public:
  Os(SparkboxLevelInterface& sparkbox) : LevelInterface(sparkbox) {}
  const char* Run() final;
};

}  // namespace sparkbox::level::os
