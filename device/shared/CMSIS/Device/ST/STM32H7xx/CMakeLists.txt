cmake_minimum_required(VERSION 3.16)

add_library(cmsis.device.st.stm32h7xx INTERFACE)
target_include_directories(cmsis.device.st.stm32h7xx SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)

target_link_libraries(cmsis.device.st.stm32h7xx
    INTERFACE
        cmsis
        stm32h7_hal_interface

)