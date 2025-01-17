cmake_minimum_required(VERSION 3.16)

add_library(device.host.audio STATIC
    host_audio_driver.cc
)

target_include_directories(device.host.audio
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(device.host.audio
    PUBLIC
        device.app.driver
        sparkbox
        sparkbox.audio.driver
        sparkbox.core
)