#include <iostream>

#include <index.h>
#include "file/file_picker.h"


int main() {
    auto ui = RootWindow::create();
    
    FilePicker picker; 
    ui->on_addFiles([&picker]() {
        std::cout << "UI Button clicked! Opening file picker..." << std::endl;
        picker.pickFiles();
    });

    ui->run(); 

    return 0;
}