cmake_minimum_required(VERSION 3.16)

# sparkbox.filesystem.driver
add_library(sparkbox.filesystem.driver INTERFACE)
target_include_directories(sparkbox.filesystem.driver
    INTERFACE
        public
)
target_link_libraries(sparkbox.filesystem.driver
    INTERFACE
        sparkbox.status
)

# sparkbox.filesystem.controller
add_library(sparkbox.filesystem.controller STATIC
    filesystem_controller.cc
)

target_include_directories(sparkbox.filesystem.controller
    PUBLIC
        public
)

target_link_libraries(sparkbox.filesystem.controller
    PUBLIC
        sparkbox.filesystem.driver
)