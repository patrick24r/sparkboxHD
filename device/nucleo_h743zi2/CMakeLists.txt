cmake_minimum_required(VERSION 3.16)

add_subdirectory(core)
add_subdirectory(filesystem)

# FreeRTOS config
set(FREERTOS_PORT "GCC_ARM_CM4F" CACHE STRING "" FORCE)
add_library(freertos_config INTERFACE)
target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)
target_compile_definitions(freertos_config INTERFACE
    projCOVERAGE_TEST=0
)
target_link_libraries(freertos_config INTERFACE
    cmsis.device.st.stm32h7xx
)

# Sparkbox hal conf
add_library(stm32h7_hal_conf INTERFACE)
target_include_directories(stm32h7_hal_conf INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/public
)

# Sparkbox device config
add_library(sparkbox_device STATIC
    sparkbox_device.cc
)
target_include_directories(sparkbox_device
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)
target_link_libraries(sparkbox_device
    PUBLIC
        nucleo_h743zi2.core
        nucleo_h743zi2.filesystem
        sparkbox
)