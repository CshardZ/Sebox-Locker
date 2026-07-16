#include <index.h>
#include<iostream>

#include "include/app_utils.h"
#include "include/ui_controller.h"


int main() {
    bool is_first_startup = ensure_default_directories();
    
    auto gui = RootWindow::create();
    UIController controller(gui, is_first_startup);
    
    controller.bind_ui_callbacks();
    controller.refresh_explorer();

    gui->run(); 

    terminate_and_clear_temp_data();
    return 0;
}