// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#include "include/ui_controller.h"
#include <fstream>
#include <slint.h>
#include "index.h"

#include "core/include/file_service.h"
#include "core/include/auth_service.h"
#include "include/utils.h"

namespace fs = std::filesystem;


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


void UIController::refresh_explorer() {
    namespace fs = std::filesystem;
    delete_decrypted_files_directory();
    fs::path base_path = get_app_data_path();
    fs::path temp_dir = base_path / "User-Data" / "decrypted";
    
    if (!fs::exists(temp_dir)) {
        fs::create_directories(temp_dir);
    }

    auto explorer_items = file_service.get_directory_contents(base_path.string());
    auto items = std::make_shared<slint::VectorModel<ExplorerItem>>();
    
    for (const auto& item : explorer_items) {
        fs::path source_path = base_path / item.name;
        fs::path final_path;
        std::string display_name = item.name;

        if (!item.is_directory) {
            display_name = fs::path(item.name).stem().string();
            fs::path dest_path = temp_dir / display_name;
            file_service.decrypt_and_copy_file(source_path.string(), dest_path.string());
            final_path = dest_path;
            items->push_back(ExplorerItem{
                .name = slint::SharedString(display_name),
                .is_directory = item.is_directory,
                .path = slint::SharedString(final_path.string())
            });
        }
    }
    gui->set_explorer_items(items);
}


// ================================================================================================
void UIController::bind_ui_callbacks() {
    // ====================================================
    gui->on_open_nfd_files_selector([this]() {
        std::cout << "Add Files - button clicked" << std::endl;
        fs::path seboxAppDataDir = get_app_data_path();
        std::vector<std::string> files = this->file_service.select_and_copy_files();
        if(!files.empty()) {
            for (const auto& pathStr : files) {
                fs::path source = pathStr;
                fs::path destination = seboxAppDataDir / (source.filename().string() + ".enc");
                
                // 2. Use the encryptor instead of fs::copy
                if (this->file_service.encrypt_and_copy_file(source.string(), destination.string())) {
                    std::cout << "Successfully encrypted: " << source.filename() << std::endl;
                } else {
                    std::cerr << "Failed to encrypt: " << source.filename() << std::endl;
                }
            }
        }
        
        std::cout << "Processing complete." << std::endl;
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
        std::cout << "Opening file: " << file_path << '\n';
        this->file_service.view_file(file_path);
    });
    // ====================================================
    gui->on_password_created([this](slint::SharedString user_input) {
        std::string password = std::string(user_input);
        std::cout << "Password Submitted: " << password << '\n';
        std::string hashed_password = AuthService::hash_password(password);
        std::cout << "Hashed Password: " << hashed_password << '\n';
        // create a file -> store password -> encrypt file : required for password verification on next login
        fs::path seboxAppDataDir = get_app_data_path();
        fs::path passwordFile = seboxAppDataDir / "hashed_password.txt";
        std::ofstream out(passwordFile, std::ios::binary);
        if (!out) {
            throw std::runtime_error("Failed to create password file");
        }
        out << hashed_password;
        out.close();
        gui->set_is_first_startup(false);
        gui->set_is_authenticated(false);
    });
    // ====================================================
    gui->on_login_submitted([this](slint::SharedString user_input) {
        std::string password = std::string(user_input);
        std::cout << "Password Submitted: " << password << '\n';
        std::string hashed_password = AuthService::hash_password(password);
        std::cout << "Hashed Password: " << hashed_password << '\n';

        // create a file -> store password -> encrypt file : required for password verification on next login
        fs::path seboxAppDataDir = get_app_data_path();
        fs::path passwordFile = seboxAppDataDir / "hashed_password.txt";
        std::ofstream out(passwordFile, std::ios::binary);
        if (!out) {
            throw std::runtime_error("Failed to create password file");
        }
        out << hashed_password;
        out.close();

        gui->set_is_authenticated(true);
    });
    // ====================================================
}
// ================================================================================================