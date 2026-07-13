// ----------------------------------------------
// - Files and folder picker/drager utility
// ----------------------------------------------
#pragma once
#include <string>
#include <vector>


class FilePicker {
    public:
        std::vector<std::string> pickFiles();
        std::string pickFolder();
};