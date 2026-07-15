// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#include <slint.h>
#include "index.h"
#include "include/ui_controller.h"
#include "include/utils.h"
#include "core/include/file_picker.h"
#include "core/include/file_explorer.h"
#include "core/include/file_encryptor.h"

namespace fs = std::filesystem;


std::vector<unsigned char> myKey = {
    1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32
};


// Constructor
UIController::UIController(slint::ComponentHandle<RootWindow> gui) : gui(gui), picker(), explorer(), encryptor(myKey) {
    // Used intializer list
    // Body Empty
}

// Destructor
UIController::~UIController() {}


void UIController::refresh_explorer() {
    namespace fs = std::filesystem;
    cleanupTempDirectory();
    fs::path base_path = getAppDataDirectory();
    fs::path temp_dir = base_path / "temp";
    
    if (!fs::exists(temp_dir)) {
        fs::create_directories(temp_dir);
    }

    auto explorer_items = explorer.list_directory(base_path);
    auto items = std::make_shared<slint::VectorModel<ExplorerItem>>();
    
    for (const auto& item : explorer_items) {
        fs::path source_path = base_path / item.name;
        fs::path final_path;
        std::string display_name = item.name;

        if (!item.is_directory) {
            display_name = fs::path(item.name).stem().string();
            fs::path dest_path = temp_dir / display_name;
            encryptor.decrypt_file(source_path, dest_path);
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
        // std::vector<unsigned char> myKey(crypto_secretstream_xchacha20poly1305_KEYBYTES, 0); 

        fs::path seboxAppDataDir = getAppDataDirectory();
        std::vector<std::string> files = this->picker.pick_files();
        if(!files.empty()) {
            for (const auto& pathStr : files) {
                fs::path source = pathStr;
                fs::path destination = seboxAppDataDir / (source.filename().string() + ".enc");
                
                // 2. Use the encryptor instead of fs::copy
                if (this->encryptor.encrypt_file(source, destination)) {
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
        fs::path seboxAppDataDir = getAppDataDirectory();
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
    gui->on_open_file([this](slint::SharedString path) {
        std::string file_path = std::string(path);
        std::cout << "Opening file: " << file_path << '\n';
        this->explorer.open_file(file_path);
    });
    // ====================================================
    gui->on_login_submitted([this](slint::SharedString user_input) {
        std::cout << "Password Submitted: " << user_input << '\n';
        gui->set_is_authenticated(true);
    });
    // ====================================================
}
// ================================================================================================