// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <sodium.h>
#include "core/include/file_service.h"
#include "core/include/core_utils.h"

namespace fs = std::filesystem;
using namespace std;


bool ensure_default_directories() {
    fs::path sebox_root_folder = get_app_data_path();
    fs::path sebox_data_folder = sebox_root_folder / "Sebox-Data";
    fs::path user_data_folder = sebox_root_folder / "User-Data";
    bool is_first_startup = !fs::exists(sebox_data_folder);
    
    vector<fs::path> paths_to_create = {
        sebox_data_folder,
        sebox_data_folder / "keys",
        user_data_folder / "decrypted",
        // user_data_folder / "encrypted", // will be re-created on every app termination with updated data
        user_data_folder / "backup"
    };
    
    for (const auto& path : paths_to_create) {
        if (!fs::exists(path)) {
            fs::create_directories(path);
        }
    }
    return is_first_startup;
}


void save_updates_to_encrypted_folder(FileService& file_service) {
    fs::path root = get_app_data_path() / "User-Data";
    fs::path enc_dir = root / "encrypted";
    fs::path dec_dir = root / "decrypted";

    fs::create_directory(enc_dir);
    auto explorer_items = file_service.get_directory_contents(dec_dir.string());
    
    for (const auto& item : explorer_items) {
        if (item.is_directory) continue;
        fs::path source = dec_dir / item.name;
        fs::path destination = enc_dir / (item.name + ".enc");
        file_service.encrypt_and_copy_file(source.string(), destination.string());
    }

    std::cout << "\n\nLatest files encrypted and saved" << std::endl;
}


void terminate_and_clear_temp_data(FileService& file_service) {
    std::cout<<"\n\n\ APp cleanup in progress...."<<std::endl;
    fs::path root = get_app_data_path() / "User-Data";
    fs::path dec_dir = root / "decrypted";
    save_updates_to_encrypted_folder(file_service);
    delete_directory(dec_dir.string());
    std::cout<<"\n\n\ APp cleanup in complete...."<<std::endl;
}