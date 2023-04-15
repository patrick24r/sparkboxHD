cmake_minimum_required(VERSION 3.16)

add_subdirectory(audio)
add_subdirectory(filesystem)


# Sparkbox status
add_library(sparkbox.status INTERFACE)
target_include_directories(sparkbox.status
    INTERFACE
        public
)

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
        sparkbox.audio.controller
        sparkbox.filesystem.controller
)