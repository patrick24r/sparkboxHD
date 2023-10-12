set(CMAKE_CROSSCOMPILING TRUE)

set(TOOLCHAIN_PREFIX arm-none-eabi-)

set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_ASM_COMPILER ${TOOLCHAIN_PREFIX}gcc)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}g++)
set(CMAKE_OBJCOPY ${TOOLCHAIN_PREFIX}objcopy)
set(CMAKE_SIZE ${TOOLCHAIN_PREFIX}size)

set(CMAKE_EXECUTABLE_SUFFIX_ASM ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_C ".elf")
set(CMAKE_EXECUTABLE_SUFFIX_CXX ".elf")

set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(SPARKBOX_DEVICE "NUCLEO-H743ZI2" CACHE STRING "" FORCE)

# Compile flags
set(DEVICE_OPTIONS 
    -mcpu=cortex-m7
    -mthumb
    -mfpu=fpv5-d16
    -mfloat-abi=hard
)

set(COMPILE_OPTIONS
    ${DEVICE_OPTIONS}
    -fdata-sections
    -ffunction-sections
    -Wall
    -Og
    -g
    -gdwarf-2
)

add_compile_definitions(
    USE_HAL_DRIVER
    STM32H743xx
)

add_compile_options(
    ${COMPILE_OPTIONS}
)

set(CMAKE_ASM_FLAGS "-x assembler-with-cpp")

add_link_options(
    -T${CMAKE_CURRENT_LIST_DIR}/STM32H743ZITx_FLASH.ld
    ${DEVICE_OPTIONS}
    -specs=nosys.specs
    -lc
    -lm
    -lnosys
    -Wl,-Map=${PROJECT_NAME}.map,--cref
    -Wl,--gc-sections
)