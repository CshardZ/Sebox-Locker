// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include "core/include/file_service.h"
#include "core/include/core_utils.h"

namespace fs = std::filesystem;
using namespace std;


// TODO: not here
std::vector<unsigned char> tempKeyhere = { // TODO - not here
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
};

FileService file_service(tempKeyhere);


bool ensure_default_directories() {
    fs::path sebox_root_folder = get_app_data_path();
    fs::path sebox_data_folder = sebox_root_folder / "Sebox-Data";
    fs::path user_data_folder = sebox_root_folder / "User-Data";
    bool is_first_startup = !fs::exists(sebox_data_folder);
    
    vector<fs::path> paths_to_create = {
        sebox_data_folder,
        sebox_data_folder / "auth",
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


void save_updates_to_encrypted_folder() {
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


void terminate_and_clear_temp_data() {
    fs::path root = get_app_data_path() / "User-Data";
    fs::path dec_dir = root / "decrypted";
    save_updates_to_encrypted_folder();
    delete_directory(dec_dir.string());
}