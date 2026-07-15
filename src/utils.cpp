// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#include <iostream> 
#include <filesystem>
#include "include/utils.h"

namespace fs = std::filesystem;


fs::path getAppDataDirectory() {
    // Create and/or Get
    // platform specific Sebox app data directory path
    fs::path base;
    #if defined(_WIN32)
        // Windows: Use LocalAppData environment variable
        const char* appdata = std::getenv("LOCALAPPDATA");
        base = appdata ? fs::path(appdata) : fs::path(".");
    #elif defined(__APPLE__)
        // macOS: Standard user library path
        base = fs::path(std::getenv("HOME")) / "Library/Application Support";
    #else
        // Linux: Follows XDG standard
        const char* home = std::getenv("HOME");
        base = home ? (fs::path(home) / ".local/share") : fs::path(".");
    #endif

    fs::path seboxAppDataDir = base / "Sebox";
    std::error_code ec;
    if (!fs::exists(seboxAppDataDir)) {
        if (!fs::create_directories(seboxAppDataDir, ec)) {
            std::cout << "Failed to create directory: " << ec.message() << std::endl;
        }
    }
    std::cout <<"Created/Retrieved App data path: " << seboxAppDataDir << std::endl;
    return seboxAppDataDir;
}