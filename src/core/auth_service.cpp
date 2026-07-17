// ----------------------------------------------
// - Auth service for app
// ----------------------------------------------
#include "include/auth_service.h"
#include "include/file_service.h"
#include "include/core_utils.h"
#include <stdexcept>
#include <sodium.h>

namespace fs = std::filesystem;


AuthService::AuthService() {
    if (sodium_init() < 0) throw std::runtime_error("Sodium init failed.");
}

std::vector<unsigned char> AuthService::encrypt_with_aead(const std::vector<unsigned char>& msg, const std::vector<unsigned char>& key) {
    std::vector<unsigned char> nonce(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES, 0);
    std::vector<unsigned char> ct(msg.size() + crypto_aead_xchacha20poly1305_ietf_ABYTES);
    unsigned long long len;

    crypto_aead_xchacha20poly1305_ietf_encrypt(ct.data(), &len, msg.data(), msg.size(), NULL, 0, NULL, nonce.data(), key.data());
    return ct;
}

std::vector<unsigned char> AuthService::decrypt_with_aead(const std::vector<unsigned char>& ct, const std::vector<unsigned char>& key) {
    std::vector<unsigned char> nonce(crypto_aead_xchacha20poly1305_ietf_NPUBBYTES, 0);
    std::vector<unsigned char> pt(ct.size() - crypto_aead_xchacha20poly1305_ietf_ABYTES);
    unsigned long long len;

    if (crypto_aead_xchacha20poly1305_ietf_decrypt(pt.data(), &len, NULL, ct.data(), ct.size(), NULL, 0, nonce.data(), key.data()) != 0)
        throw std::runtime_error("Decryption failed.");
    return pt;
}



bool AuthService::verify_login(FileService& file_service, const std::string& raw_password) {
    fs::path root = get_app_data_path() / "Sebox-Data" / "keys";
    try {
        // Explicitly convert char vector to unsigned char vector
        auto salt_raw = file_service.read_file((root / "salt.bin").string());
        std::vector<unsigned char> salt(salt_raw.begin(), salt_raw.end());
        
        auto enc_key_raw = file_service.read_file((root / "master_key.bin").string());
        std::vector<unsigned char> encrypted_master_key(enc_key_raw.begin(), enc_key_raw.end());

        auto password_key = derive_key(raw_password, salt);
        auto master_key = decrypt_with_aead(encrypted_master_key, password_key);
        
        file_service.set_key(master_key);
        return true;
    } catch (...) {
        return false;
    }
}



std::vector<unsigned char> AuthService::derive_key(const std::string& pwd, const std::vector<unsigned char>& salt) {
    if (salt.size() != crypto_pwhash_SALTBYTES) throw std::invalid_argument("Bad salt.");
    std::vector<unsigned char> key(32);
    if (crypto_pwhash(key.data(), key.size(), pwd.c_str(), pwd.length(), salt.data(), 
        crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_ARGON2ID13) != 0)
        throw std::runtime_error("Derivation failed.");
    return key;
}

void AuthService::create_and_inject_master_key(FileService& file_service, const std::string& pwd) {
    std::vector<unsigned char> mk(32), salt(crypto_pwhash_SALTBYTES);
    randombytes_buf(mk.data(), mk.size());
    randombytes_buf(salt.data(), salt.size());
    
    auto pk = derive_key(pwd, salt);
    auto emk = encrypt_with_aead(mk, pk);
    
    fs::path root = get_app_data_path() / "Sebox-Data" / "keys";
    file_service.write_file((root / "salt.bin").string(), salt);
    file_service.write_file((root / "master_key.bin").string(), emk);
    file_service.set_key(mk); 
    
    sodium_memzero(mk.data(), mk.size());
    sodium_memzero(pk.data(), pk.size());
}