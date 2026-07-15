// ----------------------------------------------
// - Auth service for app
// ----------------------------------------------
#include "include/auth_service.h"
#include <stdexcept>
#include <iostream>

AuthService::AuthService() {
    if (sodium_init() < 0) {
        throw std::runtime_error("libsodium could not be initialized.");
    }
}

std::string AuthService::hash_password(const std::string& password) {
    char hashed_password[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(
            hashed_password, 
            password.c_str(), password.length(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE, 
            crypto_pwhash_MEMLIMIT_INTERACTIVE
        ) != 0) {
    
            throw std::runtime_error("Out of memory during password hashing.");
    }

    return std::string(hashed_password);
}

bool AuthService::verify_password(const std::string& password, const std::string& stored_hash) {
    return crypto_pwhash_str_verify(
        stored_hash.c_str(), 
        password.c_str(), password.length()
    ) == 0;
}

std::vector<unsigned char> AuthService::derive_key(const std::string& password, const std::vector<unsigned char>& salt) {
    if (salt.size() != crypto_pwhash_SALTBYTES) {
        throw std::invalid_argument("Invalid salt length.");
    }

    std::vector<unsigned char> key(32); // 32 bytes

    if (crypto_pwhash(
            key.data(), key.size(),
            password.c_str(), password.length(),
            salt.data(),
            crypto_pwhash_OPSLIMIT_INTERACTIVE, 
            crypto_pwhash_MEMLIMIT_INTERACTIVE,
            crypto_pwhash_ALG_ARGON2ID13) != 0) {
        throw std::runtime_error("Key derivation failed.");
    }

    return key;
}
