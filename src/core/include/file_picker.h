// ----------------------------------------------
// - Files and folder picker/drager utility
// ----------------------------------------------
#pragma once
#include <string>
#include <vector>


class FilePicker {
    public:
        std::vector<std::string> pick_files();
        std::string pick_folder();
};