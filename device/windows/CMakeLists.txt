cmake_minimum_required(VERSION 3.16)

set(CMAKE_TOOLCHAIN_FILE
    ${CMAKE_CURRENT_SOURCE_DIR}/windows.cmake)

add_library(sparkbox_device STATIC
    sparkbox_device.cc
)

target_include_directories(sparkbox_device
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sparkbox_device
    PUBLIC
        device.shared.host.controller
        device.shared.host.filesystem
        sparkbox
)