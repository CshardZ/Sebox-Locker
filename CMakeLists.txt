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

# --- Slint project template boilerplate
find_package(Slint QUIET)

if (NOT Slint_FOUND)
  message("Slint could not be located in the CMake module search path. Downloading it from Git and building it locally")
  include(FetchContent)
  FetchContent_Declare(
    Slint
    GIT_REPOSITORY https://github.com/slint-ui/slint.git
    # `release/1` will auto-upgrade to the latest Slint >= 1.0.0 and < 2.0.0
    # `release/1.0` will auto-upgrade to the latest Slint >= 1.0.0 and < 1.1.0
    GIT_TAG release/1
    SOURCE_SUBDIR api/cpp
  )
  FetchContent_MakeAvailable(Slint)
endif (NOT Slint_FOUND)

target_link_libraries(Sebox PRIVATE Slint::Slint)
slint_target_sources(Sebox ui/app-window.slint)

# On Windows, copy the Slint DLL next to the application binary so that it's found.
if (WIN32)
    add_custom_command(TARGET Sebox POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_RUNTIME_DLLS:Sebox> $<TARGET_FILE_DIR:Sebox> COMMAND_EXPAND_LISTS)
endif()
