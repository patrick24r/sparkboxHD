cmake_minimum_required(VERSION 3.16)

# Sparkbox core
add_library(sparkbox.core INTERFACE)
target_include_directories(sparkbox.core
    INTERFACE
        public
)