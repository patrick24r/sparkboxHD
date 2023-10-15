cmake_minimum_required(VERSION 3.16)

add_library(nucleo_h743zi2.filesystem
    filesystem_driver.cc
)
target_include_directories(nucleo_h743zi2.filesystem
    PUBLIC
        public
)
target_link_libraries(nucleo_h743zi2.filesystem
    PUBLIC
        nucleo_h743zi2.core_defines
        sparkbox.filesystem.driver
        stm32h7_hal
)