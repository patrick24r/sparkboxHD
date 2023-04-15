cmake_minimum_required(VERSION 3.16)

if (NOT SPARKBOX_DEVICE)
    if (UNIX)
        message(WARNING "Auto-Detected Posix, setting SPARKBOX_DEVICE=POSIX")
        set(SPARKBOX_DEVICE POSIX CACHE STRING "Sparkbox device")
    elseif(WIN32)
        message(WARNING "Auto-Detected Windows, setting SPARKBOX_DEVICE=WIN32")
        set(SPARKBOX_DEVICE WIN32 CACHE STRING "Sparkbox device")
    else ()
        message(FATAL_ERROR "SPARKBOX_DEVICE not selected.")
    endif()
endif()

# Add the subdirectory of the specified device
if (${SPARKBOX_DEVICE} STREQUAL POSIX)
    add_subdirectory(posix)
elseif (${SPARKBOX_DEVICE} STREQUAL WIN32)
    add_subdirectory(windows)
else ()
    message(FATAL_ERROR "Invalid SPARKBOX_DEVICE")
endif()

add_subdirectory(shared)