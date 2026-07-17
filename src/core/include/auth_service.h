// ----------------------------------------------
// - Auth service for app
// ----------------------------------------------
#pragma once
#include <vector>
#include <string>
#include <sodium.h>
#include "file_service.h"


class AuthService {
public:
    AuthService();

    static std::vector<unsigned char> decrypt_with_aead(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key);
    static std::vector<unsigned char> encrypt_with_aead(const std::vector<unsigned char>& message, const std::vector<unsigned char>& key);
    static bool verify_login(FileService& file_service, const std::string& raw_password);

    // Derives a cryptographic key from a password and a salt
    // 'salt' must be crypto_pwhash_SALTBYTES long
    static std::vector<unsigned char> derive_key(const std::string& password, const std::vector<unsigned char>& salt);
    // Utility to securely clear sensitive data from memory
    static void secure_clear(std::vector<unsigned char>& data);
    // Create master key and inject to file service
    static void create_and_inject_master_key(FileService& file_service, const std::string& raw_password);
};