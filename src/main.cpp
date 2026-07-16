#include <index.h>
#include "include/ui_controller.h"
#include "include/utils.h"
#include<iostream>


int main() {
    bool is_first_startup = ensure_default_directories();
    
    auto gui = RootWindow::create();
    UIController controller(gui, is_first_startup);
    std::cout<<"Passed main is_first_startup: " << is_first_startup <<std::endl;
    
    controller.bind_ui_callbacks();
    controller.refresh_explorer();

    gui->run(); 
    delete_decrypted_files_directory();
    return 0;
}