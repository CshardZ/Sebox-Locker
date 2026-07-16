# --- Minimum CMAKE version required
cmake_minimum_required(VERSION 3.25)

# --- Project meta
project(
    Sebox
    VERSION 0.1.0
    LANGUAGES C CXX
)

# --- C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Executable
add_executable(
    Sebox
    src/core/file_service.cpp
    src/core/auth_service.cpp
    src/core/core_utils.cpp
    src/app_utils.cpp
    src/ui_controller.cpp
    src/main.cpp
)

# --- CPM
include(cmake/CPM.cmake)

# --- Slint
find_package(Slint QUIET)
if(NOT Slint_FOUND)
    CPMAddPackage(
        NAME Slint
        GITHUB_REPOSITORY slint-ui/slint
        GIT_TAG release/1
        SOURCE_SUBDIR api/cpp
    )
endif()

slint_target_sources(Sebox ui/index.slint)

if(WIN32)
    add_custom_command(
        TARGET Sebox POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
            $<TARGET_RUNTIME_DLLS:Sebox>
            $<TARGET_FILE_DIR:Sebox>
        COMMAND_EXPAND_LISTS
    )
endif()

# --- vcpkg packages
find_package(nfd CONFIG REQUIRED)
find_package(unofficial-sodium CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# --- Includes
target_include_directories(Sebox PRIVATE
    src
)

# --- Link libraries
target_link_libraries(Sebox PRIVATE
    Slint::Slint
    nfd::nfd
    unofficial-sodium::sodium
    nlohmann_json::nlohmann_json
)