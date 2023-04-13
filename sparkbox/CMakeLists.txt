cmake_minimum_required(VERSION 3.16)

add_subdirectory(filesystem)

add_library(sparkbox STATIC
    sparkbox.cc
)

target_include_directories(sparkbox
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(sparkbox
    PUBLIC
        sparkbox.filesystem.controller
        sparkbox.filesystem.driver
)