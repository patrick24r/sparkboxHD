#include "host_core_driver.h"

#include "device/host/dynamic_loader.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace device::host {

void CoreDriver::SetUp(void) { SP_LOG_DEBUG("Host core driver set up..."); }

void CoreDriver::TearDown(void) {
  SP_LOG_DEBUG("Host core driver tear down...");
}

void* CoreDriver::LoadSparkbox() {
  return host::LoadLibrary("../../lib/sparkbox/libsparkbox.so");
}

sparkbox::SparkboxInterface* CoreDriver::CreateSparkbox(
    void* handle, sparkbox::CoreDriver& core_driver,
    sparkbox::filesystem::FilesystemDriver& fs_driver,
    sparkbox::audio::AudioDriver& audio_driver,
    sparkbox::controller::ControllerDriver& controller_driver) {
  using fn_type =
      sparkbox::SparkboxInterface* (*)(sparkbox::CoreDriver&,
                                       sparkbox::filesystem::FilesystemDriver&,
                                       sparkbox::audio::AudioDriver&,
                                       sparkbox::controller::ControllerDriver&);
  fn_type create_sparkbox;

  void* symbol = host::GetLibrarySymbol(handle, "CreateSparkbox");
  if (symbol == nullptr) return nullptr;

  create_sparkbox = reinterpret_cast<fn_type>(symbol);
  return create_sparkbox(core_driver, fs_driver, audio_driver,
                         controller_driver);
}

void CoreDriver::UnloadSparkbox(void* handle) { host::UnloadLibrary(handle); }

}  // namespace device::host