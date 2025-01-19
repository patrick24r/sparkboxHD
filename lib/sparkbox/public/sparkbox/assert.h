#pragma once

// If there is no special assert, use the built-in
#ifndef SP_ASSERT
#include <cassert>
#define SP_ASSERT(condition) assert(condition)
#endif