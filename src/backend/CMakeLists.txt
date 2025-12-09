cmake_minimum_required(VERSION 3.10)
project(64BoxBackend C)
 
set(CMAKE_C_STANDARD 99)
 
include_directories(include)
 
# On Windows, export all symbols automatically from the DLL so we don't
# need __declspec(dllexport) annotations in the C code.
if (WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()
 
# Core backend library for 64Box v0.1 (build as a DLL)
add_library(64box_backend SHARED
    src/emulator.c
    src/cpu.c
    src/memory.c
    src/timer.c
    src/backend_api.c
    src/video.c
    # Per-CPU cores
    src/cpu/cpu_8086.c
    src/cpu/cpu_8088.c
    src/cpu/cpu_80186.c
    src/cpu/cpu_386.c
    src/cpu/cpu_486.c
)
 
# Add more source files as they are created