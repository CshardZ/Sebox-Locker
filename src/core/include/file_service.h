// ----------------------------------------------
// - File Services:
//      - File picker
//      - File encryption/decryption
//      - File CRUD operations
// ----------------------------------------------
#pragma once
#include <string>
#include <vector>


struct FileSystemEntry {
    std::string name;
    std::string path;
    bool is_directory;
};

class FileService {
    private:
        std::vector<unsigned char> sodium_key;

    public:
        // Constructor
        explicit FileService(const std::vector<unsigned char>& key);
        // Destructor
        ~FileService(); 
        
        // Methods
        std::vector<std::string> select_and_copy_files();
        std::string select_and_copy_folder();   

        bool encrypt_and_copy_file(const std::string& source, const std::string& destination);
        bool decrypt_and_copy_file(const std::string& source, const std::string& destination);

        std::vector<FileSystemEntry> get_directory_contents(const std::string& directory_path);
        void create_file(const std::string& filepath);
        std::vector<char> read_file(const std::string& filepath);
        void write_file(const std::string& filepath, const std::vector<char>& data);
        void view_file(const std::string& filePath);
        void delete_file(const std::string& filepath);
};