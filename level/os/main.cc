#include <cstdlib>	
#include <iostream>
#include <string>

#include "sparkbox/sparkbox.h"
#include "sparkbox_device.h"

namespace {
using namespace ::std;
using namespace ::sparkbox;
}

int main(void) {
  Sparkbox sbox = device::GetSparkbox();
  sbox.Filesystem().RunTest();
  return 0;
}