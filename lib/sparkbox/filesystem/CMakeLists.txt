cmake_minimum_required(VERSION 3.26)

# sparkbox.filesystem.driver
add_library(sparkbox.filesystem.driver INTERFACE)
target_include_directories(sparkbox.filesystem.driver
    INTERFACE
        public
)
target_link_libraries(sparkbox.filesystem.driver
    INTERFACE
        sparkbox.utils
)

# sparkbox.filesystem.manager_interface
add_library(sparkbox.filesystem.manager_interface INTERFACE)
target_include_directories(sparkbox.filesystem.manager_interface
    INTERFACE
        public
)
target_link_libraries(sparkbox.filesystem.manager_interface
    INTERFACE
        sparkbox.utils
)

# sparkbox.filesystem.manager
add_library(sparkbox.filesystem.manager STATIC
    filesystem_manager.cc
)
target_include_directories(sparkbox.filesystem.manager
    PUBLIC
        public
)
target_link_libraries(sparkbox.filesystem.manager
    PUBLIC
        sparkbox.filesystem.driver
)