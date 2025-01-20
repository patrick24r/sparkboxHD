#pragma once

#include "device/app/application_driver.h"
#include "sparkbox/audio/audio_driver.h"
#include "sparkbox/controller/controller_driver.h"
#include "sparkbox/core/core_driver.h"
#include "sparkbox/filesystem/filesystem_driver.h"
#include "sparkbox/level/level_interface.h"
#include "sparkbox/sparkbox_interface.h"
#include "sparkbox/status.h"

namespace device::host {

class CoreDriver final : public device::app::CoreAppDriver {
 public:
  void SetUp() final;
  void TearDown() final;

  void* LoadLibrary(const char* library_file) final;
  void* GetLibrarySymbol(void* handle, const char* symbol) final;
  void UnloadLibrary(void* handle) final;
};

}  // namespace device::host