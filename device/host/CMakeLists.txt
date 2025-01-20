cmake_minimum_required(VERSION 3.16)

add_subdirectory(audio)
add_subdirectory(core)
add_subdirectory(controller)
add_subdirectory(filesystem)
add_subdirectory(video)

# FreeRTOS config
add_library(freertos_config INTERFACE)
target_compile_definitions(freertos_config INTERFACE
    projCOVERAGE_TEST=0
)
target_compile_options(freertos_config INTERFACE
  -fPIC
)
if (WIN32)
  set(FREERTOS_PORT "MSVC_MINGW" CACHE STRING "" FORCE)
  set(HOST_SUB_DIR "windows")
  
elseif(UNIX)
  set(FREERTOS_PORT "GCC_POSIX" CACHE STRING "" FORCE)
  set(HOST_SUB_DIR "unix")
else()
  message(FATAL_ERROR "Unknown host environment")
endif()

target_include_directories(freertos_config SYSTEM INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/${HOST_SUB_DIR}
)

add_library(device.host.dynamic_loader STATIC
  ${HOST_SUB_DIR}/dynamic_loader.cc
)
target_include_directories(device.host.dynamic_loader
  PUBLIC
    ${HOST_SUB_DIR}
)
target_link_libraries(device.host.dynamic_loader
  PUBLIC
    sparkbox.utils
    ${CMAKE_DL_LIBS}
)

# Device library
add_library(sparkbox_device STATIC
    sparkbox_device.cc
)

target_include_directories(sparkbox_device
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../
        ${HOST_SUB_DIR}
)

target_link_libraries(sparkbox_device
    PUBLIC
        device.app.driver
        device.host.audio
        device.host.controller
        device.host.core
        device.host.filesystem
        sparkbox.sparkbox_interface
)