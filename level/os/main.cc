#include <cstdlib>
#include <iostream>
#include <string>

#include "sparkbox/sparkbox.h"
#include "sparkbox_device.h"

namespace {

using ::sparkbox::Sparkbox;

}

int main(void) {
  Sparkbox sbox = device::GetSparkbox();
  sbox.SetUp();

  // Start the sparkbox
  sbox.Start();

  sbox.TearDown();
  return 0;
}