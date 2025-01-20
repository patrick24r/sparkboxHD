cmake_minimum_required(VERSION 3.26)

add_library(sparkbox_os SHARED
  os.cc
)
target_include_directories(sparkbox_os
  PUBLIC
    public
)
target_link_libraries(sparkbox_os
  PUBLIC
    sparkbox.level.level_interface
    sparkbox.sparkbox_interface
    sparkbox.utils
)

add_custom_command(
  TARGET sparkbox_os
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/sounds
    ${CMAKE_CURRENT_BINARY_DIR}/sounds
)