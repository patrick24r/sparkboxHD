cmake_minimum_required(VERSION 3.16)

if (NOT SPARKBOX_DEVICE)
    if (UNIX)
        message(WARNING "Auto-Detected Unix, setting SPARKBOX_DEVICE=UNIX")
        set(SPARKBOX_DEVICE UNIX CACHE STRING "Sparkbox device")
    else ()
        message(FATAL_ERROR "SPARKBOX_DEVICE not selected.")
    endif()
endif()

# Add the subdirectory of the specified device
if (${SPARKBOX_DEVICE} STREQUAL UNIX)
    add_subdirectory(posix)
else ()
    message(FATAL_ERROR "Invalid SPARKBOX_DEVICE")
endif()