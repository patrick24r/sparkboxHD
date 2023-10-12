cmake_minimum_required(VERSION 3.16)

add_library(nucleo_h743zi2.core
    core_driver.cc
    startup_stm32h743xx.s
    stm32h7xx_hal_msp.cc
    system_stm32h7xx.c
)
target_include_directories(nucleo_h743zi2.core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/public
)
target_link_libraries(nucleo_h743zi2.core
    PUBLIC
        cmsis.device.st.stm32h7xx
        sparkbox.core
        stm32h7_hal
)