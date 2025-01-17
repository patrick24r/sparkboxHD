cmake_minimum_required(VERSION 3.16)

add_executable(device.app
  main.cc
)

target_link_libraries(device.app
  PUBLIC
    sparkbox
    sparkbox_device
)


add_library(device.app.driver INTERFACE)
target_include_directories(device.app.driver
  INTERFACE
    public
)
target_link_libraries(device.app.driver
  INTERFACE
    sparkbox.filesystem.driver
    sparkbox.core
    sparkbox.audio.driver
    sparkbox.controller.driver
)