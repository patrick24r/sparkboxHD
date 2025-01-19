cmake_minimum_required(VERSION 3.16)

add_library(device.host.controller STATIC
    host_controller_driver.cc
)

target_include_directories(device.host.controller
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.host.controller
    PUBLIC
        device.app.driver
        sparkbox.controller.driver
        sparkbox.utils
)