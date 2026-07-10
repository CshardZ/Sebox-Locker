# --- Minimum CMAKE version required (actual used: cmake version 4.4.0-rc3)
cmake_minimum_required(VERSION 3.25)

# --- Project meta
project(
    Sebox 
    VERSION 0.1.0 
    LANGUAGES C CXX
)

# --- CPM executable path
include(cmake/CPM.cmake)

# --- Set and enforce C++ language version (not the compiler version - MSVC compiler)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)


add_executable(
    Sebox
    src/main.cpp
)