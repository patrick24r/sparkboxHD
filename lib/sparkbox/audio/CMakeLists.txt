cmake_minimum_required(VERSION 3.26)

# sparkbox.audio.driver
add_library(sparkbox.audio.driver INTERFACE)
target_include_directories(sparkbox.audio.driver
    INTERFACE
        public
)

# sparkbox.audio.manager_interface
add_library(sparkbox.audio.manager_interface INTERFACE)
target_include_directories(sparkbox.audio.manager_interface
    INTERFACE
        public
)
target_link_libraries(sparkbox.audio.manager_interface
    INTERFACE
        sparkbox.utils
)

# sparkbox.audio.manager
add_library(sparkbox.audio.manager STATIC
    audio_file_importer.cc    
    audio_manager.cc
    channel.cc
    resampler.cc
)
target_include_directories(sparkbox.audio.manager
    PUBLIC
        public
)
target_link_libraries(sparkbox.audio.manager
    PUBLIC
        freertos_kernel
        sparkbox.audio.driver
        sparkbox.core.manager
        sparkbox.filesystem.driver
        sparkbox.utils
        
)