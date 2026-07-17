// ----------------------------------------------
// - app utilities and helpers
// ----------------------------------------------
#pragma once
#include "core/include/file_service.h"


bool ensure_default_directories();
void save_updates_to_encrypted_folder(FileService& file_service);
void terminate_and_clear_temp_data(FileService& file_service);