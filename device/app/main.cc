
#include "device/app/application_driver.h"
#include "sparkbox_device.h"

int main() {
  device::app::CoreAppDriver& core_driver = device::GetCoreDriver();
  device::app::FilesystemAppDriver& filesystem_driver =
      device::GetFilesystemDriver();
  device::app::AudioAppDriver& audio_driver = device::GetAudioDriver();
  device::app::ControllerAppDriver& controller_driver =
      device::GetControllerDriver();

  // Set up the device
  core_driver.SetUp();
  filesystem_driver.SetUp();
  audio_driver.SetUp();
  controller_driver.SetUp();

  // Load the sparkbox library

  // Create a sparkbox

  // Run the sparkbox. This launches FreeRTOS and

  // Tear down device if the sparkbox is done
  controller_driver.TearDown();
  audio_driver.TearDown();
  filesystem_driver.TearDown();
  core_driver.TearDown();
  return 0;
}
