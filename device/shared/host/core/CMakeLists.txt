cmake_minimum_required(VERSION 3.16)

add_library(device.shared.host.core STATIC
    host_core_driver.cc
)

target_include_directories(device.shared.host.core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.shared.host.core
    PUBLIC
        sparkbox.core
)