#pragma once

#include "sparkbox/sparkbox.h"

namespace {

using ::sparkbox::Sparkbox;

}

namespace device {

Sparkbox& GetSparkbox();

} // namespace device