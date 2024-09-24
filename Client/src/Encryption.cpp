#include "Encryption.h"
#include <iostream>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <vector>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>


Encryption::Encryption() {
    // Constructor definition
}

Encryption::~Encryption() {
    // Destructor definition
}

std::vector<unsigned char> Encryption::StringToVector(const std::string& str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}

std::string Encryption::VectorToString(const std::vector<unsigned char>& vec) {
    return std::string(vec.begin(), vec.end());
}

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <iostream>
#include <vector>
#include <cstring>

bool Encryption::AESEncrypt(const std::vector<unsigned char>& plaintext, const std::vector<unsigned char>& key,
                std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& iv, std::vector<unsigned char>& tag) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    int len = 0;
    int ciphertext_len = 0;
    
    // Generate a random IV of 16 bytes
    iv.resize(16);
    if (!RAND_bytes(iv.data(), iv.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Initialize encryption operation with AES-256-GCM
    if (!EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Set IV length
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Initialize key and IV
    if (!EVP_EncryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Encrypt the message
    ciphertext.resize(plaintext.size());
    if (!EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len = len;

    // Finalize encryption
    if (!EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    ciphertext_len += len;
    ciphertext.resize(ciphertext_len);

    // Get the tag (16 bytes)
    tag.resize(16);
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

bool Encryption::AESDecrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                 const std::vector<unsigned char>& iv, const std::vector<unsigned char>& tag,
                 std::vector<unsigned char>& plaintext) {
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    if (!ctx) return false;

    int len = 0;
    int plaintext_len = 0;

    // Initialize decryption operation with AES-256-GCM
    if (!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Set IV length
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, iv.size(), nullptr)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Initialize key and IV
    if (!EVP_DecryptInit_ex(ctx, nullptr, nullptr, key.data(), iv.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Decrypt the ciphertext
    plaintext.resize(ciphertext.size());
    if (!EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }
    plaintext_len = len;

    // Set the expected tag value
    if (!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, tag.size(), (void*)tag.data())) {
        EVP_CIPHER_CTX_free(ctx);
        return false;
    }

    // Finalize decryption
    if (!EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len)) {
        EVP_CIPHER_CTX_free(ctx);
        return false;  // Decryption failed, possibly due to tag mismatch
    }
    plaintext_len += len;
    plaintext.resize(plaintext_len);

    EVP_CIPHER_CTX_free(ctx);
    return true;
}

