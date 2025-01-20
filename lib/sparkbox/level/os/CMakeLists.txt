cmake_minimum_required(VERSION 3.16)

add_library(sparkbox.level.os SHARED
  os.cc
)
target_include_directories(sparkbox.level.os
  PUBLIC
    public
)
target_link_libraries(sparkbox.level.os
  PUBLIC
    sparkbox.level.level_interface
    sparkbox.sparkbox_interface
    sparkbox.utils
)