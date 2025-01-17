#include <cstdlib>
#include <iostream>
#include <string>

#include "sparkbox_device.h"

int main() {
  // Set up device drivers
  device::GetCoreDriver().SetUp();
  device::GetFilesystemDriver().SetUp();
  device::GetAudioDriver().SetUp();
  device::GetControllerDriver().SetUp();

  // Load the sparkbox library, then run it
  return 0;
}
