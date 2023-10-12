cmake_minimum_required(VERSION 3.16)

if (${SPARKBOX_DEVICE} STREQUAL NUCLEO-H743ZI2)
    add_subdirectory(ST)
    add_subdirectory(CMSIS)
endif()

if (${SPARKBOX_DEVICE} STREQUAL POSIX 
    OR ${SPARKBOX_DEVICE} STREQUAL WINDOWS)
    add_subdirectory(host)
endif()

