cmake_minimum_required(VERSION 3.16)

add_library(device.shared.host.controller STATIC
    host_controller_driver.cc
)

target_include_directories(device.shared.host.controller
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.shared.host.controller
    PUBLIC
        sparkbox
        sparkbox.controller.driver
)