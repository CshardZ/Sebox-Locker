#include <iostream>

#include <index.h>
#include "file/file_picker.h"


int main() {
    auto ui = RootWindow::create();
    
    FilePicker picker; 
    ui->on_open_nfd_files_selector([&picker]() {
        std::cout << "Add Files - button clicked" << std::endl;
        picker.pickFiles();
    });

    ui->on_open_nfd_folder_selector([&picker]() {
        std::cout <<"Add Folder - button clicked" << std::endl;
        picker.pickFolder();
    });

    ui->run(); 

    return 0;
}