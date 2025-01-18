#include "dynamic_loader.h"

#include <dlfcn.h>

#include "sparkbox/log.h"

namespace device::host {

void* LoadLibrary(const char* library_file) {
  void* handle = dlopen(library_file, RTLD_LAZY);
  if (handle == nullptr) {
    SP_LOG_ERROR("Error loading lib: %s", dlerror());
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