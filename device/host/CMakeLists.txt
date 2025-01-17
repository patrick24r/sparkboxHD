cmake_minimum_required(VERSION 3.16)

add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(controller)
add_subdirectory(filesystem)
add_subdirectory(video)

if (WIN32)
  add_subdirectory(windows)
elseif(UNIX)
  add_subdirectory(unix)
else()
  message(FATAL_ERROR "Unknown Host environment")
endif()


# Device library
add_library(sparkbox_device STATIC
    sparkbox_device.cc
)

target_include_directories(sparkbox_device
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../
)

target_link_libraries(sparkbox_device
    PUBLIC
        device.app.driver
        device.host.audio
        device.host.controller
        device.host.core
        device.host.filesystem
        sparkbox
)