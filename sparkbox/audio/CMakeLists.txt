cmake_minimum_required(VERSION 3.16)

# sparkbox.audio.driver
add_library(sparkbox.audio.driver INTERFACE)
target_include_directories(sparkbox.audio.driver
    INTERFACE
        public
)

# sparkbox.audio.manager
add_library(sparkbox.audio.manager STATIC
    audio_manager.cc
)

target_include_directories(sparkbox.audio.manager
    PUBLIC
        public
)

target_link_libraries(sparkbox.audio.manager
    PUBLIC
        sparkbox.audio.driver
        sparkbox.core
)