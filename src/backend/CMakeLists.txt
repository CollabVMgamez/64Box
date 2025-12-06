cmake_minimum_required(VERSION 3.10)
project(64BoxBackend C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_library(64box_backend STATIC src/emulator.c)

# Add more source files as they are created