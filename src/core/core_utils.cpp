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




// ================================================================================================