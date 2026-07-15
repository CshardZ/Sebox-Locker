#pragma once

#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;


struct DirectoryItem {
    std::string name;
    bool is_directory;
    std::string path;
};

class FileExplorer {
public:
    std::vector<DirectoryItem> list_directory(const fs::path& directoryPath);
    void open_file(const fs::path& filePath);
};