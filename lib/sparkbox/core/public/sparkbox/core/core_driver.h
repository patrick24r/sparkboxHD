#pragma once

namespace sparkbox {

class CoreDriver {
 public:
  virtual ~CoreDriver() = default;

  virtual void* LoadLibrary(const char* library_file) = 0;
  virtual void* GetLibrarySymbol(void* handle, const char* symbol) = 0;
  virtual void UnloadLibrary(void* handle) = 0;
};

}  // namespace sparkbox