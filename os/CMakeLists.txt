cmake_minimum_required(VERSION 3.16)

add_executable(os
    main.cc
)

target_link_libraries(os
    PUBLIC
        sparkbox
        sparkbox_device
)
