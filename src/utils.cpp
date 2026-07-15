// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#include "include/utils.h"
#include <iostream> 
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;


fs::path getAppDataDirectory() {
    // Create and/or Get
    // platform specific Sebox app data directory path
    fs::path base;
    #if defined(_WIN32)
        // Windows: Use LocalAppData environment variable
        const char* appdata = getenv("LOCALAPPDATA");
        base = appdata ? fs::path(appdata) : fs::path(".");
    #elif defined(__APPLE__)
        // macOS: Standard user library path
        base = fs::path(getenv("HOME")) / "Library/Application Support";
    #else
        // Linux: Follows XDG standard
        const char* home = getenv("HOME");
        base = home ? (fs::path(home) / ".local/share") : fs::path(".");
    #endif

    fs::path seboxAppDataDir = base / "Sebox";
    error_code ec;
    if (!fs::exists(seboxAppDataDir)) {
        if (!fs::create_directories(seboxAppDataDir, ec)) {
            cout << "Failed to create directory: " << ec.message() << endl;
        }
    }
    cout <<"Created/Retrieved App data path: " << seboxAppDataDir << endl;
    return seboxAppDataDir;
}

void cleanupTempDirectory() {
    fs::path temp_dir = getAppDataDirectory() / "temp";
    if (fs::exists(temp_dir)) {
        error_code ec;
        if (fs::remove_all(temp_dir, ec)) {
            cout << "Successfully cleaned temp directory." << endl;
        } else if (ec) {
            cerr << "Error cleaning temp directory: " << ec.message() << endl;
        }
    }
}