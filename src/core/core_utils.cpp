// ----------------------------------------------
// - core module utilities and helpers
// ----------------------------------------------
#include "include/core_utils.h"
#include <iostream> 
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;


// ================================================================================================
fs::path get_app_data_path() {
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

    fs::path app_data_path = base / "Sebox";
    error_code ec;
    if (!fs::exists(app_data_path)) {
        if (!fs::create_directories(app_data_path, ec)) {
            cout << "Failed to create directory: " << ec.message() << endl;
        }
    }
    cout <<"Created/Retrieved App data path: " << app_data_path << endl;
    return app_data_path;
}


void create_backup(const std::string& source, const std::string& destination) {
    try {
        fs::path src_path(source);
        fs::path dest_path = fs::path(destination) / "Sebox_Locker_BACKUP";
        if (!fs::exists(src_path) || !fs::is_directory(src_path)) {
            throw std::runtime_error("Source directory does not exist or is not a directory.");
        }
        if (!fs::exists(dest_path)) {
            fs::create_directories(dest_path);
        }

        // copy_options::recursive: Copies subdirectories and their contents
        // copy_options::overwrite_existing: Overwrites files if they already exist in dest
        fs::copy(src_path, dest_path, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
        std::cout << "Backup completed successfully." << std::endl;

    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "General error: " << e.what() << std::endl;
    }
}
// ================================================================================================




// ================================================================================================
void create_directory(const string& directory_path) {
    fs::path to_create_path(directory_path);
    if (!fs::exists(to_create_path)) {
        fs::create_directories(to_create_path);
        cout<<"create_directory(): "<<to_create_path<<endl;
    }
}

void delete_directory(const string& directory_path) {
    fs::path to_delete_path(directory_path);
    if (fs::exists(to_delete_path)) {
        fs::remove_all(to_delete_path);
        cout<<"delete_directory(): "<<to_delete_path<<endl;
    }
}
// ================================================================================================