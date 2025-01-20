cmake_minimum_required(VERSION 3.26)

add_executable(device.app
  main.cc
)
target_link_libraries(device.app
  PUBLIC
    sparkbox_device
    sparkbox.sparkbox_interface
)

add_library(device.app.driver INTERFACE)
target_include_directories(device.app.driver
  INTERFACE
    public
)
target_link_libraries(device.app.driver
  INTERFACE
    sparkbox.filesystem.driver
    sparkbox.utils
    sparkbox.audio.driver
    sparkbox.controller.driver
    sparkbox.core.driver
)