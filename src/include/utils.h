// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#pragma once
#include <filesystem>


std::filesystem::path get_app_data_path();
void delete_decrypted_files_directory();
void create_decrypted_files_directory();
bool ensure_default_directories();