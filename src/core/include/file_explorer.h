#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;


struct DirectoryItem {
    std::string name;
    bool is_directory;
};

class FileExplorer {
public:
    std::vector<DirectoryItem> list_directory(const fs::path& directoryPath);
};