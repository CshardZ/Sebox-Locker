// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#pragma once
#include <slint.h>
#include "index.h"
#include "core/include/file_picker.h"
#include "core/include/file_explorer.h"
#include "core/include/file_encryptor.h"


class UIController {
    public:
        UIController(slint::ComponentHandle<RootWindow> gui);
        ~UIController();
        void bind_ui_callbacks();
        void refresh_explorer();

        slint::ComponentHandle<RootWindow> gui;
        FilePicker picker;
        FileExplorer explorer;
        FileEncryptor encryptor;
};