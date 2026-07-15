// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#include <slint.h>
#include "index.h"
#include "include/ui_controller.h"
#include "include/utils.h"
#include "core/include/file_picker.h"
#include "core/include/file_explorer.h"

namespace fs = std::filesystem;


// Constructor
UIController::UIController(slint::ComponentHandle<RootWindow> gui) : gui(gui), picker(), explorer() {
    // Used intializer list
    // Body Empty
}

// Destructor
UIController::~UIController() {}


void UIController::refresh_explorer() {
    auto explorer_items = explorer.list_directory(getAppDataDirectory());
    auto items = std::make_shared<slint::VectorModel<ExplorerItem>>();
    
    for (const auto& item : explorer_items) {
        items->push_back(ExplorerItem{
            .name = slint::SharedString(item.name),
            .is_directory = item.is_directory,
            .path = slint::SharedString(item.path)
        });
    }
    gui->set_explorer_items(items);
}


// ================================================================================================
void UIController::bind_ui_callbacks() {
    // ====================================================
    gui->on_open_nfd_files_selector([this]() {
        std::cout << "Add Files - button clicked" << std::endl;
        fs::path seboxAppDataDir = getAppDataDirectory();
        std::vector<std::string> files = this->picker.pick_files();
        if(!files.empty()) {
            for (const auto& pathStr : files) {
                fs::path source = pathStr;
                fs::copy(source, seboxAppDataDir / source.filename());
            }
        }
        std::cout<<"Files Have been copied to appdata directory"<<std::endl;
        this->refresh_explorer();
    });
    // ====================================================
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
    // ====================================================
    gui->on_open_file([this](slint::SharedString path) {
        std::string file_path = std::string(path);
        std::cout << "Opening file: " << file_path << '\n';
        this->explorer.open_file(file_path);
    });
    // ====================================================
}
// ================================================================================================