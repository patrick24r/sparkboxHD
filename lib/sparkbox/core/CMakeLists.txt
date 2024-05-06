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