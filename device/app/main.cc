
#include "device/app/application_driver.h"
#include "sparkbox/log.h"
#include "sparkbox/sparkbox_interface.h"
#include "sparkbox/sparkbox_lib.h"
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

  // Load the sparkbox library. Do this specifically after setting up the
  // filesystem so the library can be read from it
  void* handle = core_driver.LoadLibrary("sparkbox/libsparkbox");
  if (handle == nullptr) {
    SP_LOG_ERROR("Failed to load libsparkbox, aborting...");
    return -1;
  }

  // Create a sparkbox
  using create_sparkbox_t = decltype(&::CreateSparkbox);
  create_sparkbox_t create_sparkbox = reinterpret_cast<create_sparkbox_t>(
      core_driver.GetLibrarySymbol(handle, "CreateSparkbox"));
  if (create_sparkbox == nullptr) {
    SP_LOG_ERROR("Failed to find CreateSparkbox symbol, aborting...");
    return -1;
  }
  sparkbox::SparkboxDeviceInterface* sparkbox = create_sparkbox(
      core_driver, filesystem_driver, audio_driver, controller_driver);
  if (sparkbox == nullptr) {
    SP_LOG_ERROR("Failed to create sparkbox, aborting...");
    return -1;
  }

  // Run the sparkbox. This launches FreeRTOS and the scheduler. We don't
  // expect to continue past this
  sparkbox->SetUp();
  sparkbox->Start();

  // If somehow we make it here, tear down everything
  sparkbox->TearDown();
  core_driver.UnloadLibrary(handle);

  // Tear down device if the sparkbox is done
  controller_driver.TearDown();
  audio_driver.TearDown();
  filesystem_driver.TearDown();
  core_driver.TearDown();
  return 0;
}
