cmake_minimum_required(VERSION 3.26)

add_subdirectory(audio)
add_subdirectory(controller)
add_subdirectory(core)
add_subdirectory(level)
add_subdirectory(filesystem)

# Sparkbox core
add_library(sparkbox.utils INTERFACE)
target_include_directories(sparkbox.utils
  INTERFACE
    public
)
target_compile_options(sparkbox.utils INTERFACE
  -Wall
  -Werror
  -ffile-prefix-map=${CMAKE_SOURCE_DIR}=.
  $<$<COMPILE_LANGUAGE:CXX>:
    -fno-exceptions
    -std=c++2a
  >
  -fPIC
)

# Sparkbox manager base class that all managers inherit from
add_library(sparkbox.manager STATIC
  manager.cc
  router.cc
  task.cc
)
target_include_directories(sparkbox.manager
  PUBLIC
    public
  PRIVATE
    internal
)
target_link_libraries(sparkbox.manager
  PUBLIC
    freertos_kernel
    sparkbox.utils
)


add_library(sparkbox.sparkbox_interface INTERFACE)
target_include_directories(sparkbox.sparkbox_interface
    INTERFACE
        public
)
target_link_libraries(sparkbox.sparkbox_interface
    INTERFACE
        sparkbox.audio.driver
        sparkbox.audio.manager_interface
        sparkbox.controller.driver
        sparkbox.controller.manager_interface
        sparkbox.core.driver
        sparkbox.filesystem.driver
        sparkbox.filesystem.manager_interface
        sparkbox.utils
)

# Main sparkbox library
add_library(sparkbox SHARED
    sparkbox.cc
)
target_include_directories(sparkbox
    PUBLIC
        public
)
target_link_libraries(sparkbox
    PUBLIC
        freertos_kernel
        sparkbox.audio.manager
        sparkbox.controller.manager
        sparkbox.manager
        sparkbox.filesystem.manager
        sparkbox.utils
)