cmake_minimum_required(VERSION 3.16)

add_library(nucleo_h743zi2.core
    core_driver.cc
    clock.cc
    interrupt.cc
    pin.cc
    startup_stm32h743xx.s
    system_stm32h7xx.c
)
target_include_directories(nucleo_h743zi2.core
    PUBLIC
        public
)
target_link_libraries(nucleo_h743zi2.core
    PUBLIC
        cmsis.device.st.stm32h7xx
        sparkbox.core
        stm32h7_ll
)