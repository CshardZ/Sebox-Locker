// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#pragma once
#include <slint.h>
#include "index.h"
#include "core/include/file_picker.h"
#include "core/include/file_explorer.h"


class UIController {
    public:
        UIController(slint::ComponentHandle<RootWindow> gui);
        ~UIController();
        void bind_ui_callbacks();
        void refresh_explorer();

        slint::ComponentHandle<RootWindow> gui;
        FilePicker picker;
        FileExplorer explorer;
};