cmake_minimum_required(VERSION 3.16)

add_subdirectory(audio)
add_subdirectory(controller)
add_subdirectory(core)
add_subdirectory(filesystem)

add_library(sparkbox_interface INTERFACE)
target_include_directories(sparkbox_interface
    INTERFACE
        public
)
target_link_libraries(sparkbox_interface
    INTERFACE
        sparkbox.audio.manager_interface
        sparkbox.controller.manager_interface
        sparkbox.core
        sparkbox.filesystem.manager_interface
)


# Main sparkbox library
add_library(sparkbox SHARED
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