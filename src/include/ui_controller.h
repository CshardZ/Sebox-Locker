// ----------------------------------------------
// - Main UI/GUI controller logic
// ----------------------------------------------
#pragma once
#include <slint.h>
#include "index.h"
#include "core/include/file_service.h"


class UIController {
    public:
        UIController(slint::ComponentHandle<RootWindow> gui, bool is_first_startup);
        ~UIController();
        void bind_ui_callbacks();
        void refresh_explorer();
        slint::ComponentHandle<RootWindow> gui;

        FileService file_service;
};