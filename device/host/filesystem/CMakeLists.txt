cmake_minimum_required(VERSION 3.26)

add_library(device.host.filesystem STATIC
    host_filesystem_driver.cc
)

target_include_directories(device.host.filesystem
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.host.filesystem
    PUBLIC
        device.app.driver
        sparkbox.filesystem.driver
        sparkbox.utils
)