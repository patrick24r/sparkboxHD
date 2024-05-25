cmake_minimum_required(VERSION 3.16)

# Sparkbox core
add_library(sparkbox.core INTERFACE)
target_include_directories(sparkbox.core
  INTERFACE
    public
)

target_compile_options(sparkbox.core INTERFACE
  -Wall
  -Werror
  -ffile-prefix-map=${CMAKE_SOURCE_DIR}=.
  $<$<COMPILE_LANGUAGE:CXX>:
    -fno-exceptions
    -std=c++2a
  >
)

# Sparkbox manager base class that all managers inherit from
add_library(sparkbox.core.task STATIC
  task.cc
)
target_include_directories(sparkbox.core.task
  PUBLIC
    public
)
target_link_libraries(sparkbox.core.task
  PUBLIC
    freertos_kernel
    sparkbox.core
)

# Sparkbox manager base class that all managers inherit from
add_library(sparkbox.core.manager STATIC
  manager.cc
)
target_include_directories(sparkbox.core.manager
  PUBLIC
    internal
)
target_link_libraries(sparkbox.core.manager
  PUBLIC
    freertos_kernel
    sparkbox.core
    sparkbox.core.task
)


