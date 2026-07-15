// ----------------------------------------------
// - Auth service for app
// ----------------------------------------------
#pragma once
#include <vector>
#include <string>
#include <sodium.h>


class AuthService {
public:
    AuthService();

    // Hashes a new password and returns the storage-ready string
    static std::string hash_password(const std::string& password);

    // Verifies a raw password against a stored hash string
    static bool verify_password(const std::string& password, const std::string& stored_hash);

    // Derives a cryptographic key from a password and a salt
    // 'salt' must be crypto_pwhash_SALTBYTES long
    static std::vector<unsigned char> derive_key(const std::string& password, const std::vector<unsigned char>& salt);

    // Utility to securely clear sensitive data from memory
    static void secure_clear(std::vector<unsigned char>& data);
};