cmake_minimum_required(VERSION 3.16)

add_library(device.shared.host.filesystem STATIC
    host_filesystem_driver.cc
)

target_include_directories(device.shared.host.filesystem
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.shared.host.filesystem
    PUBLIC
        sparkbox
        sparkbox.filesystem.driver
)