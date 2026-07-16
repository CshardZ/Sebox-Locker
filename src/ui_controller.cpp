// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#include "include/ui_controller.h"
#include <string>
#include <vector>
#include <fstream>
#include<iostream>
#include <slint.h>
#include "index.h"

#include "core/include/file_service.h"
#include "core/include/auth_service.h"
#include "core/include/core_utils.h"

namespace fs = std::filesystem;


// ================================================================================================
// Constructor
UIController::UIController(slint::ComponentHandle<RootWindow> gui, bool is_first_startup, FileService& fs) 
    : gui(gui), file_service(fs) 
{
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
        std::string filename = fs::path(item.name).filename().string();
        std::string display_name;
        if (filename.ends_with(".enc")) {
            display_name = filename.substr(0, filename.size() - 4); // remove .enc extension for display
        } else {
            display_name = filename;
        }

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
        AuthService::create_and_inject_master_key(this->file_service, password);  
        gui->set_is_first_startup(false);
        gui->set_is_authenticated(false);
    });
    // ====================================================
    gui->on_login_submitted([this](slint::SharedString user_input) {
        std::string password = std::string(user_input);
        bool matched = AuthService::verify_login(this->file_service, password);
        if(matched) {
            gui->set_is_authenticated(true);
            this->refresh_explorer();
        } else {
            gui->set_password_placeholder("Wrong Password");
        }
    });
    // ====================================================
}
// ================================================================================================