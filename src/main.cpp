#include <iostream>

#include <index.h>
#include "file/file_picker.h"

#include <filesystem>
#include <string>
#include <vector>
#include <cstdlib>

namespace fs = std::filesystem;

fs::path GetAppStorageDir() {
    /* 
    - Create and/or Get
    - platform specific app data directory path
    */
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
            std::cerr << "Failed to create directory: " << ec.message() << std::endl;
        }
    }
    std::cout <<"Created/Retrieved App data path: " << seboxAppDataDir << std::endl;
    return seboxAppDataDir;
}


int main() {
    auto ui = RootWindow::create();

    
    FilePicker picker; 
    ui->on_open_nfd_files_selector([&picker]() {
        std::cout << "Add Files - button clicked" << std::endl;
        fs::path seboxAppDataDir = GetAppStorageDir(); // create and/or get
        std::vector<std::string> files = picker.pickFiles();
        if(!files.empty()) {
            for (const auto& pathStr : files) {
                fs::path source = pathStr;
                fs::copy(source, seboxAppDataDir / source.filename());
            }
        }
        std::cout<<"Files Have been copied to appdata directory"<<std::endl;
    });
    
    ui->on_open_nfd_folder_selector([&picker]() {
        std::cout <<"Add Folder - button clicked" << std::endl;
        fs::path seboxAppDataDir = GetAppStorageDir(); // create and/or get
        std::string folder = picker.pickFolder();
        if(!folder.empty()) {
            fs::path source = folder;
            fs::copy(
                source, seboxAppDataDir / source.filename(), 
                fs::copy_options::recursive | fs::copy_options::overwrite_existing
            );
        }
        std::cout<<"Folder has been copied to appdata directory"<<std::endl;
    });

    ui->run(); 

    return 0;
}