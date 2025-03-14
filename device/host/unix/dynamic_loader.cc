#include "device/host/dynamic_loader.h"

#include <dlfcn.h>

#include <string>

#include "sparkbox/log.h"

namespace device::host {

void* LoadLibrary(const char* library_file) {
  std::string lib_file = std::string(library_file);
  // Unix library files are .so, append that here
  lib_file += ".so";
  void* handle = dlopen(lib_file.c_str(), RTLD_LAZY);
  if (handle == nullptr) {
    SP_LOG_ERROR("%s", dlerror());
  }
  return handle;
}

void* GetLibrarySymbol(void* handle, const char* symbol) {
  return dlsym(handle, symbol);
}

void UnloadLibrary(void* handle) {
  if (handle == nullptr) return;
  dlclose(handle);
}

}  // namespace device::host