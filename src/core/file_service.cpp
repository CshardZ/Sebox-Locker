// ----------------------------------------------
// - File Services:
//      - File picker
//      - File encryption/decryption
//      - File CRUD operations
// ----------------------------------------------
#include "include/file_service.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <nfd.h>
#include <sodium.h>

namespace fs = std::filesystem;
using namespace std;


// ================================================================================================
FileService::FileService(const vector<unsigned char>& key) {
    if (sodium_init() < 0)
        throw runtime_error("libsodium could not be initialized.");
    if (key.size() != crypto_secretstream_xchacha20poly1305_KEYBYTES)
        throw invalid_argument("Invalid key size. Must be 32 bytes.");
    sodium_key = key;
}

FileService::~FileService() {
    if (!sodium_key.empty()) {
        sodium_memzero(sodium_key.data(), sodium_key.size());
    }
}

// ================================================================================================
vector<string> FileService::select_and_copy_files() {
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
// ================================================================================================
string FileService::select_and_copy_folder() {
    NFD_Init();
    string folder;
    nfdu8char_t* outPath = nullptr;
    nfdresult_t result = NFD_PickFolderU8(&outPath, nullptr);
    
    if (result == NFD_OKAY) {
        folder = string(outPath);
        cout << "Selected Folder: " << folder << endl;
        NFD_FreePathU8(outPath);
    }
    
    NFD_Quit();
    return folder;
}
// ================================================================================================




// ================================================================================================
vector<FileSystemEntry> FileService::get_directory_contents(const string& directory_path) {
    vector<FileSystemEntry> items;
    fs::path path(directory_path);

    if (fs::exists(path) && fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            items.push_back({
                entry.path().filename().string(), 
                entry.path().string(),
                entry.is_directory()
            });
        }
    }

    return items;
}
// ================================================================================================
void FileService::view_file(const string& file_path) {
    /* Open file in system configured apps */
    fs::path path(file_path);
    string command;

    #ifdef _WIN32
        // Windows: "start" command
        command = "start \"\" \"" + path.string() + "\"";
    #elif __APPLE__
        // macOS: "open" command
        command = "open \"" + path.string() + "\"";
    #else
        // Linux: "xdg-open" command
        command = "xdg-open \"" + path.string() + "\"";
    #endif

    system(command.c_str());
}
// ================================================================================================
void FileService::create_file(const string& file_path) {
    fs::path path(file_path);
    fs::create_directories(path.parent_path());
    // Create an empty file
    // ofstream creates the file if it doesn't exist
    ofstream ofs(path);
    cout <<"File created: " << file_path << endl;

}
// ================================================================================================
void FileService::delete_file(const string& file_path) {
    fs::path path(file_path);
    // fs::remove returns true if the file existed and was removed
    fs::remove(path);
    cout <<"File deleted: " << file_path << endl;
}
// ================================================================================================
vector<char> FileService::read_file(const string& filepath) {
    ifstream file(filepath, ios::binary | ios::ate);
    
    if (!file.is_open()) {
        cerr << "Could not open file for reading: " << filepath << endl;
        return {};
    }
    streamsize size = file.tellg();
    file.seekg(0, ios::beg);

    vector<char> buffer(size);
    if (file.read(buffer.data(), size)) {
        return buffer;
    }
    return {};
}

// ================================================================================================
void FileService::write_file(const string& filepath, const vector<char>& data) {
    ofstream file(filepath, ios::binary);
    
    if (!file.is_open()) {
        // Get the last system error
        std::error_code ec = std::make_error_code(static_cast<std::errc>(errno));
        std::cerr << "Failed to open: " << filepath 
                << " | Error: " << ec.message() << std::endl;
    }

    file.write(data.data(), data.size());
    file.close();
}
// ================================================================================================




// ================================================================================================
bool FileService::encrypt_and_copy_file(const string& source, const string& destination) {
    fs::path src_path(source);
    fs::path dst_path(destination);
    ifstream in(src_path, ios::binary);
    ofstream out(dst_path, ios::binary);

    if (!in || !out) return false;

    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    
    // Start encryption and write header to file
    crypto_secretstream_xchacha20poly1305_init_push(&state, header, sodium_key.data());
    out.write((char*)header, sizeof(header));

    // Buffer setup
    unsigned char chunk[4096];
    unsigned char encrypted[4096 + 17]; // 17 is the extra overhead for authentication
    unsigned long long out_len;

    // Read and encrypt loop
    while (in.read((char*)chunk, sizeof(chunk)) || in.gcount() > 0) {
        size_t count = in.gcount();
        unsigned char tag = in.eof() ? crypto_secretstream_xchacha20poly1305_TAG_FINAL : 0;
        crypto_secretstream_xchacha20poly1305_push(&state, encrypted, &out_len, chunk, count, nullptr, 0, tag);
        out.write((char*)encrypted, out_len);
    }

    return true;
}
// ================================================================================================
bool FileService::decrypt_and_copy_file(const string& source, const string& destination) {
    fs::path src_path(source);
    fs::path dst_path(destination);
    ifstream in(src_path, ios::binary);
    ofstream out(dst_path, ios::binary);

    if (!in || !out) return false;

    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    
    in.read((char*)header, sizeof(header));
    if (in.gcount() != sizeof(header)) 
        return false;
    if (crypto_secretstream_xchacha20poly1305_init_pull(&state, header, sodium_key.data()) != 0) 
        return false;

    unsigned char chunk[4096 + 17];
    unsigned char decrypted[4096];
    unsigned long long out_len;
    unsigned char tag;

    while (in.read((char*)chunk, sizeof(chunk)) || in.gcount() > 0) {
        size_t count = in.gcount();
        
        if (crypto_secretstream_xchacha20poly1305_pull(&state, decrypted, &out_len, &tag, chunk, count, nullptr, 0) != 0) {
            return false; // Authentication failed (wrong key or file tampered)
        }
        
        out.write((char*)decrypted, out_len);
    }

    return true;
}
// ================================================================================================
