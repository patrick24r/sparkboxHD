cmake_minimum_required(VERSION 3.16)

add_subdirectory(os)

add_library(sparkbox.level.level_interface INTERFACE)
target_include_directories(sparkbox.level.level_interface
  INTERFACE
    public
)
target_link_libraries(sparkbox.level.level_interface
  INTERFACE
    sparkbox.sparkbox_interface   
)