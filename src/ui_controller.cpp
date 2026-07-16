// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#include "include/ui_controller.h"
#include <string>
#include <vector>
#include <fstream>
#include <slint.h>
#include "index.h"

#include "core/include/file_service.h"
#include "core/include/auth_service.h"
#include "core/include/core_utils.h"

namespace fs = std::filesystem;


// TODO: use std namespace and remove all std:: prefixes
// ================================================================================================
std::vector<unsigned char> tempKey = { // TODO - not here
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
};

// Constructor
UIController::UIController(slint::ComponentHandle<RootWindow> gui, bool is_first_startup) : gui(gui), file_service(tempKey) {
    this->gui->set_is_first_startup(is_first_startup);
}

// Destructor
UIController::~UIController() {}
// ================================================================================================




// ================================================================================================
void UIController::refresh_explorer() {
    fs::path root = get_app_data_path() / "User-Data";
    fs::path enc_dir = root / "encrypted";
    fs::path dec_dir = root / "decrypted";

    auto items = std::make_shared<slint::VectorModel<ExplorerItem>>();
    bool decrypting = fs::is_directory(enc_dir);
    fs::path source_dir = decrypting ? enc_dir : dec_dir;

    auto explorer_items = file_service.get_directory_contents(source_dir.string());

    for (const auto& item : explorer_items) {
        if (item.is_directory) continue;

        fs::path final_path = source_dir / item.name;
        std::string display_name = fs::path(item.name).stem().string();

        if (decrypting) {
            final_path = dec_dir / display_name;
            file_service.decrypt_and_copy_file((source_dir / item.name).string(), final_path.string());
        }

        items->push_back(ExplorerItem{
            .name = slint::SharedString(display_name),
            .is_directory = item.is_directory,
            .path = slint::SharedString(final_path.string())
        });
    }

    if (decrypting) {
        delete_directory(enc_dir.string());
    }

    gui->set_explorer_items(items);
}
// ================================================================================================




// ================================================================================================
void UIController::bind_ui_callbacks() {
    // ====================================================
    gui->on_open_nfd_files_selector([this]() {
        std::cout << "Add Files - button clicked" << std::endl;
        fs::path sebox_root_folder = get_app_data_path();
        fs::path user_decrypted_folder = sebox_root_folder / "User-Data" / "decrypted";
        std::vector<std::string> files = this->file_service.select_and_copy_files();
        if(!files.empty()) {
            for (const auto& pathStr : files) {
                fs::path source = pathStr;
                fs::path destination = user_decrypted_folder / (source.filename().string());
                fs::copy(source, destination);
            }
        }
        
        std::cout << "File Dialog Processing complete. \n\n" << std::endl;
        this->refresh_explorer();
    });
    // ====================================================
    /*
    gui->on_open_nfd_folder_selector([this]() {
        std::cout <<"Add Folder - button clicked" << std::endl;
        fs::path seboxAppDataDir = get_app_data_path();
        std::string folder = this->picker.pick_folder();
        if(!folder.empty()) {
            fs::path source = folder;
            fs::copy(
                source, seboxAppDataDir / source.filename(), 
                fs::copy_options::recursive | fs::copy_options::overwrite_existing
            );
        }
        std::cout<<"Folder has been copied to appdata directory"<<std::endl;
        this->refresh_explorer();
    });
    */
    // ====================================================
    gui->on_view_file([this](slint::SharedString path) {
        std::string file_path = std::string(path);
        this->file_service.view_file(file_path);
    });
    // ====================================================
    gui->on_password_created([this](slint::SharedString user_input) {
        std::string password = std::string(user_input);
        fs::path sebox_auth_folder = get_app_data_path() / "Sebox-Data" / "auth";
        fs::path password_file = sebox_auth_folder / "hashed_password.txt";
        fs::path password_file_encrypted = password_file;
        password_file_encrypted += ".enc";

        std::string hashed_password = AuthService::hash_password(password);
        std::vector<char> password_vector(hashed_password.begin(), hashed_password.end());
        this->file_service.write_file(password_file.string(), password_vector);
        this->file_service.encrypt_and_copy_file(password_file.string(), password_file_encrypted.string());
        this->file_service.delete_file(password_file.string());
        
        gui->set_is_first_startup(false);
        gui->set_is_authenticated(false);
    });
    // ====================================================
    gui->on_login_submitted([this](slint::SharedString user_input) {
        std::string password = std::string(user_input);
        fs::path sebox_auth_folder = get_app_data_path() / "Sebox-Data" / "auth";
        fs::path password_file = sebox_auth_folder / "hashed_password.txt";
        fs::path password_file_encrypted = password_file;
        password_file_encrypted += ".enc";
        this->file_service.decrypt_and_copy_file(password_file_encrypted.string(), password_file.string());
        std::vector<char> hashed_password_vector = this->file_service.read_file(password_file.string());
        std::string hashed_password_string(hashed_password_vector.begin(), hashed_password_vector.end());
        bool matched = AuthService::verify_password(password, hashed_password_string);
        if(matched) {
            gui->set_is_authenticated(true);
        } else {
            gui->set_password_placeholder("Wrong Password");
        }
        this->file_service.delete_file(password_file.string());
    });
    // ====================================================
}
// ================================================================================================