set(CMAKE_CROSSCOMPILING TRUE)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)

add_compile_options(
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
    -fdata-sections
    -ffunction-sections
    -Og
    -g 
    -gdwarf-2
)

# Set link options for the whole project
add_link_options(
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32H743ZITx_FLASH.ld
)

add_compile_definitions(
    USE_HAL_DRIVER
    STM32H743xx
)