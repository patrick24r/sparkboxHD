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
    audio_file_importer.cc
)

target_include_directories(sparkbox.audio.manager
    PUBLIC
        public
    PRIVATE
        internal
)

target_link_libraries(sparkbox.audio.manager
    PUBLIC
        freertos_kernel
        sparkbox.audio.driver
        sparkbox.core
        sparkbox.filesystem.manager
)