#include <string>
#include <vector>
#include <filesystem>
#include "include/file_explorer.h"

namespace fs = std::filesystem;


std::vector<DirectoryItem> FileExplorer::list_directory(const fs::path& directoryPath) {
    std::vector<DirectoryItem> items;
    
    if (fs::exists(directoryPath) && fs::is_directory(directoryPath)) {
        for (const auto& entry : fs::directory_iterator(directoryPath)) {
            items.push_back({
                entry.path().filename().string(), 
                entry.is_directory(),
                entry.path().string()
            });
        }
    }
    return items;
}

void FileExplorer::open_file(const fs::path& filepath) {
    std::string command;
    #ifdef _WIN32
        // Windows: "start" command
        command = "start \"\" \"" + filepath.string() + "\"";
    #elif __APPLE__
        // macOS: "open" command
        command = "open \"" + filepath.string() + "\"";
    #else
        // Linux: "xdg-open" command
        command = "xdg-open \"" + filepath.string() + "\"";
    #endif

    std::system(command.c_str());
}