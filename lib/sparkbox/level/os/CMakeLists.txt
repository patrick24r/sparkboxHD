cmake_minimum_required(VERSION 3.16)

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