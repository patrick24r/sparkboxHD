cmake_minimum_required(VERSION 3.16)

add_library(device.shared.host.audio STATIC
    host_audio_driver.cc
)

target_include_directories(device.shared.host.audio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.shared.host.audio
    PUBLIC
        sparkbox
        sparkbox.audio.driver
        sparkbox.core
        sparkbox.core.manager
)