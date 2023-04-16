cmake_minimum_required(VERSION 3.16)

# sparkbox.controller.driver
add_library(sparkbox.controller.driver INTERFACE)
target_include_directories(sparkbox.controller.driver
    INTERFACE
        public
)
target_link_libraries(sparkbox.controller.driver
    INTERFACE
        sparkbox.core
)

# sparkbox.controller.controller
add_library(sparkbox.controller.controller STATIC
    controller_controller.cc
)

target_include_directories(sparkbox.controller.controller
    PUBLIC
        public
)

target_link_libraries(sparkbox.controller.controller
    PUBLIC
        sparkbox.controller.driver
)