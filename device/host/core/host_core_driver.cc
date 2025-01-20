#include "host_core_driver.h"

#include "device/host/dynamic_loader.h"
#include "sparkbox/level/level_interface.h"
#include "sparkbox/log.h"
#include "sparkbox/status.h"

namespace device::host {

void CoreDriver::SetUp(void) { SP_LOG_DEBUG("Host core driver set up..."); }

void CoreDriver::TearDown(void) {
  SP_LOG_DEBUG("Host core driver tear down...");
}

void* CoreDriver::LoadLibrary(const char* level_name) {
  if (level_name == nullptr) return nullptr;
  return host::LoadLibrary(level_name);
}

void CoreDriver::UnloadLibrary(void* handle) {
  if (handle == nullptr) return;
  host::UnloadLibrary(handle);
}

void* CoreDriver::GetLibrarySymbol(void* handle, const char* symbol) {
  if (handle == nullptr) return nullptr;
  return host::GetLibrarySymbol(handle, symbol);
}

}  // namespace device::host