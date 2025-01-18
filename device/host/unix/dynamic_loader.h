#pragma once

namespace device::host {

void* LoadLibrary(const char* library_file);

void* GetLibrarySymbol(void* handle, const char* symbol);

void UnloadLibrary(void* handle);

}  // namespace device::host