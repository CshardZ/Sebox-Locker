#include <iostream>
#include <cstdlib>
#include <filesystem>
#include <string>
#include <vector>

#include <index.h>
#include "include/ui_controller.h"


int main() {
    auto gui = RootWindow::create();
    UIController controller(gui);
    
    controller.bind_ui_callbacks();
    controller.refresh_explorer(); // Initial load

    gui->run(); 
    return 0;
}