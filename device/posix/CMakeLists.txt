cmake_minimum_required(VERSION 3.16)

# Device FreeRTOS config
set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(freertos_config INTERFACE
    projCOVERAGE_TEST=0
)

# Device library
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
        device.shared.host.core
        device.shared.host.filesystem
        sparkbox
)