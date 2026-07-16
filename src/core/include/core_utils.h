// ----------------------------------------------
// - core module utilities and helpers
// ----------------------------------------------
#pragma once
#include <filesystem>


std::filesystem::path get_app_data_path();
void delete_directory(const std::string& directory_path);
void create_directory(const std::string& directory_path);