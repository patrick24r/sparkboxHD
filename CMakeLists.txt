cmake_minimum_required(VERSION 3.16)

project(Sparkbox C CXX)
add_compile_options(-Wall -ffile-prefix-map=${CMAKE_CURRENT_SOURCE_DIR}=)

set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -std=c++2a") 

add_subdirectory(device)
add_subdirectory(level)
add_subdirectory(lib)