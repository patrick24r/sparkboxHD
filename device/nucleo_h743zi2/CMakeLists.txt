cmake_minimum_required(VERSION 3.16)

# FreeRTOS config
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_compile_definitions(freertos_config INTERFACE
    projCOVERAGE_TEST=0
)

add_library(sparkbox_device STATIC
    sparkbox_device.cc
)

target_include_directories(sparkbox_device
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)

target_link_libraries(sparkbox_device
    PUBLIC
        sparkbox
)