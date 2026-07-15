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
                entry.is_directory()
            });
        }
    }
    return items;
}