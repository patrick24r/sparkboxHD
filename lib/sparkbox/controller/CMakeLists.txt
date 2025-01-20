cmake_minimum_required(VERSION 3.16)

# sparkbox.controller.driver
add_library(sparkbox.controller.driver INTERFACE)
target_include_directories(sparkbox.controller.driver
    INTERFACE
        public
)
target_link_libraries(sparkbox.controller.driver
    INTERFACE
        sparkbox.utils
)

add_library(sparkbox.controller.manager_interface INTERFACE)
target_include_directories(sparkbox.controller.manager_interface
    INTERFACE
        public
)
target_link_libraries(sparkbox.controller.manager_interface
    INTERFACE
        sparkbox.utils
)

# sparkbox.controller.manager
add_library(sparkbox.controller.manager STATIC
    controller_manager.cc
)

target_include_directories(sparkbox.controller.manager
    PUBLIC
        public
)

target_link_libraries(sparkbox.controller.manager
    PUBLIC
        freertos_kernel
        sparkbox.controller.driver
        sparkbox.controller.manager_interface
        sparkbox.core.manager
)