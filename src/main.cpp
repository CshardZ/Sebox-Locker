#include <index.h>
#include<iostream>

#include "core/include/file_service.h"
#include "include/ui_controller.h"
#include "include/app_utils.h"


int main() {
    bool is_first_startup = ensure_default_directories();
    FileService file_service;

    auto gui = RootWindow::create();
    UIController controller(gui, is_first_startup, file_service);
    
    controller.bind_ui_callbacks();
    
    gui->run(); 

    terminate_and_clear_temp_data(file_service);
    return 0;
}