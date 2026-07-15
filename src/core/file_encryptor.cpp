// ----------------------------------------------
// - File encryption/decryption tools
// ----------------------------------------------
#include <sodium.h>
#include <filesystem>
#include <fstream>
#include <vector>
#include "include/file_encryptor.h"


FileEncryptor::FileEncryptor(const std::vector<unsigned char>& k) : key(k) {
    if (sodium_init() < 0) throw std::runtime_error("libsodium init failed");
}

bool FileEncryptor::encrypt_file(const std::filesystem::path& source, const std::filesystem::path& destination) {
    std::ifstream in(source, std::ios::binary);
    std::ofstream out(destination, std::ios::binary);

    if (!in || !out) return false;

    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    
    // Start encryption and write header to file
    crypto_secretstream_xchacha20poly1305_init_push(&state, header, key.data());
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

bool FileEncryptor::decrypt_file(const std::filesystem::path& source, const std::filesystem::path& destination) {
    std::ifstream in(source, std::ios::binary);
    std::ofstream out(destination, std::ios::binary);

    if (!in || !out) return false;

    crypto_secretstream_xchacha20poly1305_state state;
    unsigned char header[crypto_secretstream_xchacha20poly1305_HEADERBYTES];
    
    in.read((char*)header, sizeof(header));
    if (in.gcount() != sizeof(header)) return false;

    if (crypto_secretstream_xchacha20poly1305_init_pull(&state, header, key.data()) != 0) return false;

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