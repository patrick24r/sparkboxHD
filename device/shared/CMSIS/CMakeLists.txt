cmake_minimum_required(VERSION 3.16)

add_subdirectory(Device)

add_library(cmsis INTERFACE)
target_include_directories(cmsis SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/Include
)