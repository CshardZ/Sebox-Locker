// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#include "include/utils.h"
#include <iostream> 
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;


fs::path get_app_data_path() {
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

void delete_decrypted_files_directory() {
    fs::path temp_dir = get_app_data_path() / "decrypted";
    if (fs::exists(temp_dir)) {
        error_code ec;
        if (fs::remove_all(temp_dir, ec)) {
            cout << "Successfully cleaned temp directory." << endl;
        } else if (ec) {
            cerr << "Error cleaning temp directory: " << ec.message() << endl;
        }
    }
}


bool ensure_default_directories() {
    fs::path sebox_folder = get_app_data_path();
    fs::path sebox_data_folder = sebox_folder / "Sebox-Data";
    fs::path user_data_folder = sebox_folder / "User-Data";
    bool is_first_startup = !fs::exists(sebox_data_folder);

    std::vector<fs::path> paths_to_create = {
        sebox_data_folder,
        user_data_folder / "decrypted",
        user_data_folder / "encrypted",
        user_data_folder / "backup"
    };

    for (const auto& path : paths_to_create) {
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }
    }

    cout<<"is_first_startup: " << is_first_startup <<endl;

    return is_first_startup;
}