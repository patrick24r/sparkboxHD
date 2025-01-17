cmake_minimum_required(VERSION 3.16)

if (NOT SPARKBOX_DEVICE)
    message(FATAL_ERROR "Unspecified SPARKBOX_DEVICE")
endif()

add_subdirectory(app)

# Add the subdirectory of the specified device
if (${SPARKBOX_DEVICE} STREQUAL HOST)
    add_subdirectory(host)
elseif(${SPARKBOX_DEVICE} STREQUAL NUCLEO-H743ZI2)
    add_subdirectory(nucleo_h743zi2)
else ()
    message(FATAL_ERROR "Invalid SPARKBOX_DEVICE")
endif()