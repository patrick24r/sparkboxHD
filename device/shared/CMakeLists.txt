cmake_minimum_required(VERSION 3.16)

if ((${SPARKBOX_DEVICE} STREQUAL POSIX) OR 
    (${SPARKBOX_DEVICE} STREQUAL WIN32))
    add_subdirectory(host)
endif()