#include <iostream>
#include <slint.h>

#include <index.h>
#include "core/include/file_picker.h"
#include "core/include/file_explorer.h"

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
        std::vector<std::string> files = picker.pick_files();
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
        std::string folder = picker.pick_folder();
        if(!folder.empty()) {
            fs::path source = folder;
            fs::copy(
                source, seboxAppDataDir / source.filename(), 
                fs::copy_options::recursive | fs::copy_options::overwrite_existing
            );
        }
        std::cout<<"Folder has been copied to appdata directory"<<std::endl;
    });

    // File explorer first view
    FileExplorer explorer;
    auto explorer_items = explorer.list_directory(GetAppStorageDir());
    auto items = std::make_shared<slint::VectorModel<ExplorerItem>>();
    for (const auto& item : explorer_items) {
        items->push_back(ExplorerItem{
            .name = slint::SharedString(item.name),
            .is_directory = item.is_directory,
            .path = item.path
        });
    }
    ui->set_explorer_items(items);

    ui->on_open_file([&explorer](slint::SharedString path) {
        std::string file_path = std::string(path);
        std::cout << "Opening file: " << file_path << '\n';
        explorer.open_file(file_path);
    });

    ui->run(); 

    return 0;
}