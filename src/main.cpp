#include "index.h"
#include <nfd.h>


int main() {
    auto ui = RootWindow::create();

    ui->on_addFiles([]() {
        NFD_Init();
        nfdchar_t *outPath = NULL;
        nfdresult_t result = NFD_OpenDialog(&outPath, NULL, 0, NULL);

        if (result == NFD_OKAY) {
            // Do your logic here
            NFD_FreePath(outPath);
        }
        NFD_Quit();
    });

    ui->run();
    return 0;
}