#include <nfd.h> // Ensure this path is correct in your project settings
#include <vector>
#include <string>
#include <iostream>
#include "include/file_picker.h"

using namespace std;


vector<string> FilePicker::pick_files() {
    NFD_Init();
    vector<string> files; 
    
    const nfdpathset_t* pathSet = nullptr; 
    nfdresult_t result = NFD_OpenDialogMultipleU8(&pathSet, nullptr, 0, nullptr);
    
    if(result == NFD_OKAY) {
        nfdpathsetsize_t count = 0;
        NFD_PathSet_GetCount(pathSet, &count);

        for(nfdpathsetsize_t i = 0; i < count; ++i) {
            nfdu8char_t* path = nullptr;
            if (NFD_PathSet_GetPathU8(pathSet, i, &path) == NFD_OKAY) {
                if (path) {
                    files.push_back(string(path));
                    cout << "Selected: " << path << endl;
                    NFD_PathSet_FreePathU8(path);
                }
            }
        }
        NFD_PathSet_Free(pathSet);
    }

    NFD_Quit();
    return files;
}


string FilePicker::pick_folder() {
    NFD_Init();
    std::string folder;

    nfdu8char_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolderU8(&outPath, nullptr);

    if (result == NFD_OKAY) {
        folder = std::string(outPath);
        cout << "Selected Folder: " << folder << endl;
        NFD_FreePathU8(outPath);
    }

    NFD_Quit();
    return folder;
}