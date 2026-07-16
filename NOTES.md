# Project Initial Setup Steps
- Install MSVC
- Download and install C++ for desktop environment - which bundles cmake as well
- Download CPM.cmake executable place it inside cmake/
- Setup Slint recommended project template for C++ applications
- Install Rust compiler - cmake uses cargo(a rustc package manager) to install Slint C++ package(or download package zip from official github release)
- Remove Slint android/gradel files and boilerplate codes
- Update CMakeLists if needed
- cd build -> cmake .. -> cmake --build .


# Notes
- Cmake -> CMakeLists.txt -> CPM.cmake -> MSVC compiler
- KDF: key derivation function
- High entrophy - encryption
- Low entrophy - user passwords