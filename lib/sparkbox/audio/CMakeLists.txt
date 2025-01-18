cmake_minimum_required(VERSION 3.16)

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
        sparkbox.core
)

# sparkbox.audio.manager
add_library(sparkbox.audio.manager STATIC
    audio_manager.cc
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
        sparkbox.audio.channel
        sparkbox.audio.driver
        sparkbox.audio.file_importer
        sparkbox.core
        sparkbox.core.manager
        sparkbox.filesystem.driver
        
)

# sparkbox.audio.channel
add_library(sparkbox.audio.channel STATIC
    channel.cc
)
target_include_directories(sparkbox.audio.channel
    PUBLIC
        public
        internal
)
target_link_libraries(sparkbox.audio.channel
    PUBLIC
        sparkbox.audio.resampler
        sparkbox.core
)

# sparkbox.audio.file_importer
add_library(sparkbox.audio.file_importer STATIC
    audio_file_importer.cc
)
target_include_directories(sparkbox.audio.file_importer
    PUBLIC
        public
        internal
)
target_link_libraries(sparkbox.audio.file_importer
    PUBLIC
        sparkbox.core
        sparkbox.filesystem.driver
)

# sparkbox.audio.resampler
add_library(sparkbox.audio.resampler STATIC
    resampler.cc
)
target_include_directories(sparkbox.audio.resampler
    PUBLIC
        public
        internal
)
target_link_libraries(sparkbox.audio.resampler
    PUBLIC
        sparkbox.core
)