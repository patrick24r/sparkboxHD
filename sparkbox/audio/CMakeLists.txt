cmake_minimum_required(VERSION 3.16)

# sparkbox.audio.driver
add_library(sparkbox.audio.driver INTERFACE)
target_include_directories(sparkbox.audio.driver
    INTERFACE
        public
)

# sparkbox.audio.controller
add_library(sparkbox.audio.controller STATIC
    audio_controller.cc
)

target_include_directories(sparkbox.audio.controller
    PUBLIC
        public
)

target_link_libraries(sparkbox.audio.controller
    PUBLIC
        sparkbox.audio.driver
        sparkbox.core
)