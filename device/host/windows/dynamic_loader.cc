#include "device/host/dynamic_loader.h"

#include <windows.h>

#include <codecvt>
#include <locale>
#include <string>

#include "sparkbox/log.h"

namespace device::host {

void* LoadLibrary(const char* library_file) {
  std::string lib_file = std::string(library_file);
  // Unix library files are .so, append that here
  lib_file += ".dll";

  std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
  std::wstring win_lib_file = converter.from_bytes(lib_file);

  HMODULE handle = ::LoadLibrary(win_lib_file.c_str());
  if (handle == nullptr) {
    SP_LOG_ERROR("Cannot load '%ls'", win_lib_file.c_str());
  }

  return handle;
}

void* GetLibrarySymbol(void* handle, const char* symbol) {
  return reinterpret_cast<void*>(
      GetProcAddress(static_cast<HMODULE>(handle), symbol));
}

void UnloadLibrary(void* handle) {
  if (handle == nullptr) return;
  ::FreeLibrary(static_cast<HMODULE>(handle));
}

}  // namespace device::host