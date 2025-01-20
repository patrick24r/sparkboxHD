#include "sparkbox/core/core_manager.h"

#include "sparkbox/level/level.h"
#include "sparkbox/level/level_interface.h"
#include "sparkbox/status.h"

namespace sparkbox {

const char* CoreManager::LoadAndRunLevel(const char* level_name) {
  if (level_name == nullptr) {
    SP_LOG_ERROR("Cannot load null level name");
    return nullptr;
  }

  // Try to load the level library
  void* level_library_handle = core_driver_.LoadLibrary(level_name);
  if (level_library_handle == nullptr) {
    SP_LOG_ERROR("Cannot load library '%s'", level_name);
    return nullptr;
  }

  // Try to load symbols to create and destroy a level
  void* create_level_symbol =
      core_driver_.GetLibrarySymbol(level_library_handle, "CreateLevel");
  if (create_level_symbol == nullptr) {
    SP_LOG_ERROR("Cannot load 'CreateLevel' from '%s'", level_name);
    return nullptr;
  }
  using create_fn_t = decltype(&::CreateLevel);
  create_fn_t create_level = reinterpret_cast<create_fn_t>(create_level_symbol);

  void* destroy_level_symbol =
      core_driver_.GetLibrarySymbol(level_library_handle, "DestroyLevel");
  if (destroy_level_symbol == nullptr) {
    SP_LOG_ERROR("Cannot load 'DestroyLevel' from '%s'", level_name);
    return nullptr;
  }
  using destroy_fn_t = decltype(&::DestroyLevel);
  destroy_fn_t destroy_level =
      reinterpret_cast<destroy_fn_t>(destroy_level_symbol);

  // Create the level object
  sparkbox::level::LevelInterface* level = create_level(sparkbox_);

  // Run the level
  const char* next_level_name = nullptr;
  if (level != nullptr) {
    next_level_name = level->Run();
  } else {
    SP_LOG_ERROR("Cannot run level '%s'", level_name);
  }

  // Destroy the level object
  destroy_level(level);

  // Unload the level library
  core_driver_.UnloadLibrary(level_library_handle);

  return next_level_name;
}

}  // namespace sparkbox