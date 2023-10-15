cmake_minimum_required(VERSION 3.16)

add_library(nucleo_h743zi2.core
    core_driver.cc
    startup_stm32h743xx.s
    stm32h7xx_hal_msp.c
    stm32h7xx_hal_timebase_tim.c
    stm32h7xx_it.c
    system_stm32h7xx.c
)
target_include_directories(nucleo_h743zi2.core
    PUBLIC
        public
)
target_link_libraries(nucleo_h743zi2.core
    PUBLIC
        cmsis.device.st.stm32h7xx
        nucleo_h743zi2.core_defines
        sparkbox.core
        stm32h7_hal
)

add_library(nucleo_h743zi2.core_defines INTERFACE
)
target_include_directories(nucleo_h743zi2.core_defines INTERFACE
        public
)