#pragma once

namespace sparkbox {

class CoreDriver {
 public:
  virtual ~CoreDriver() = default;

  // Load the library file. Note that library_file will not be called with a
  // suffix to support multiple architectures. Implementations need to append
  // the correct suffix
  virtual void* LoadLibrary(const char* library_file) = 0;

  // Get library symbol from an already loaded library
  virtual void* GetLibrarySymbol(void* handle, const char* symbol) = 0;

  // Unload the library
  virtual void UnloadLibrary(void* handle) = 0;
};

}  // namespace sparkbox