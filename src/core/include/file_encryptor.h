// ----------------------------------------------
// - File encryption/decryption tools
// ----------------------------------------------
#pragma once
#include <vector>
#include <filesystem>


class FileEncryptor {
private:
    std::vector<unsigned char> key;

public:
    explicit FileEncryptor(const std::vector<unsigned char>& key);
    bool encrypt_file(const std::filesystem::path& source, const std::filesystem::path& destination);
    bool decrypt_file(const std::filesystem::path& source, const std::filesystem::path& destination);
};