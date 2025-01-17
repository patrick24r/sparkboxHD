#pragma once

#include "sparkbox/sparkbox.h"

namespace sparkbox {

class Level {
 public:
  Level(sparkbox::Sparkbox& sparkbox) : sparkbox_(sparkbox) {}

  // Entrypoint for level specific game code
  virtual void Run() = 0;

 private:
  sparkbox::Sparkbox& sparkbox_;
};

}  // namespace sparkbox