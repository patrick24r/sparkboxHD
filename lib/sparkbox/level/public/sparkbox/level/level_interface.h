#pragma once

#include "sparkbox/sparkbox_interface.h"

namespace sparkbox::level {

// Class used by the sparkbox class to run the loaded levels
class LevelInterface {
 public:
  LevelInterface(SparkboxLevelInterface& sparkbox) : sparkbox_(sparkbox) {}
  virtual ~LevelInterface() = default;

  // Runs the level. Returns a string designating the next level to be run
  virtual const char* Run() = 0;

 protected:
  SparkboxLevelInterface& sparkbox_;
};

}  // namespace sparkbox::level
