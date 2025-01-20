#pragma once

#include "sparkbox/level/level_interface.h"

// Each level is compiled to a shared library. These function are to be
// implemented by each level so the sparkbox can load and execute it
extern "C" sparkbox::level::LevelInterface* CreateLevel(
    sparkbox::SparkboxLevelInterface& sparkbox);

extern "C" void DestroyLevel(sparkbox::level::LevelInterface*);