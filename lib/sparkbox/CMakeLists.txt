cmake_minimum_required(VERSION 3.16)

add_subdirectory(audio)
add_subdirectory(controller)
add_subdirectory(core)
add_subdirectory(filesystem)

# Main sparkbox library
add_library(sparkbox STATIC
    sparkbox.cc
)

target_include_directories(sparkbox
    PUBLIC
        public
)

target_link_libraries(sparkbox
    PUBLIC
        freertos_kernel
        sparkbox.audio.manager
        sparkbox.controller.manager
        sparkbox.core
        sparkbox.core.task
        sparkbox.filesystem.manager
)