cmake_minimum_required(VERSION 3.26)

add_library(sparkbox.core.driver INTERFACE)
target_include_directories(sparkbox.core.driver
  INTERFACE
    public
)
target_link_libraries(sparkbox.core.driver
  INTERFACE
    sparkbox.utils
)

add_library(sparkbox.core.manager STATIC
  core_manager.cc
)
target_include_directories(sparkbox.core.manager
  PUBLIC
    public
)
target_link_libraries(sparkbox.core.manager
  PUBLIC
    sparkbox.core.driver
    sparkbox.level.level_interface
    sparkbox.utils
)