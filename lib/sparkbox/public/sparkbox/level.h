#pragma once

#include "sparkbox/sparkbox_interface.h"

namespace sparkbox {

class Level {
 public:
  Level(sparkbox::SparkboxInterface& sparkbox) : sparkbox_(sparkbox) {}

  // Entrypoint for level specific game code
  virtual void Run() = 0;

 private:
  sparkbox::SparkboxInterface& sparkbox_;
};

}  // namespace sparkbox