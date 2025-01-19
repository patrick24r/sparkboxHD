cmake_minimum_required(VERSION 3.16)

add_library(device.host.core STATIC
    host_core_driver.cc
)

target_include_directories(device.host.core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.host.core
    PUBLIC
        device.app.driver
        sparkbox.core.driver
        sparkbox.utils
)