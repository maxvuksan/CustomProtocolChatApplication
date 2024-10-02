#include "Encryption.h"
#include <iostream>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <vector>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <cstring>

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

bool Encryption::GenerateRSAKeyPair(std::string& publicKey, std::string& privateKey) {
    int keyLength = 2048;
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    EVP_PKEY* pkey = NULL;
    
    if (!ctx || EVP_PKEY_keygen_init(ctx) <= 0 || 
        EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, keyLength) <= 0 || 
        EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        std::cerr << "Error generating RSA key pair." << std::endl;
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    // Write public key to a string
    BIO* pub_bio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PUBKEY(pub_bio, pkey)) {
        std::cerr << "Error writing public key." << std::endl;
        BIO_free(pub_bio);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }
    char* pub_key_data;
    long pub_len = BIO_get_mem_data(pub_bio, &pub_key_data);
    publicKey.assign(pub_key_data, pub_len);
    BIO_free(pub_bio);

    // Write private key to a string
    BIO* priv_bio = BIO_new(BIO_s_mem());
    if (!PEM_write_bio_PrivateKey(priv_bio, pkey, NULL, NULL, 0, NULL, NULL)) {
        std::cerr << "Error writing private key." << std::endl;
        BIO_free(priv_bio);
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }
    char* priv_key_data;
    long priv_len = BIO_get_mem_data(priv_bio, &priv_key_data);
    privateKey.assign(priv_key_data, priv_len);
    BIO_free(priv_bio);

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    
    return true;
}

bool Encryption::RSAEncrypt(const std::vector<unsigned char>& plaintext, const std::string& publicKey, std::vector<unsigned char>& ciphertext) {
    // Load public key from string
   BIO* pub_bio = BIO_new_mem_buf(publicKey.data(), -1);
    EVP_PKEY* pkey = PEM_read_bio_PUBKEY(pub_bio, NULL, NULL, NULL);
    BIO_free(pub_bio);

    if (!pkey) {
        std::cerr << "Error reading public key." << std::endl;
        return false;
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx || EVP_PKEY_encrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        std::cerr << "Error initializing encryption context." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, NULL, &outlen, plaintext.data(), plaintext.size()) <= 0) {
        std::cerr << "Error calculating encrypted size." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    ciphertext.resize(outlen);
    if (EVP_PKEY_encrypt(ctx, ciphertext.data(), &outlen, plaintext.data(), plaintext.size()) <= 0) {
        std::cerr << "Error during encryption." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    
    return true;
}

bool Encryption::RSADecrypt(const std::vector<unsigned char>& ciphertext, const std::string& privateKey, std::vector<unsigned char>& plaintext) {
    // Load private key from string
    BIO* priv_bio = BIO_new_mem_buf(privateKey.data(), -1);
    EVP_PKEY* pkey = PEM_read_bio_PrivateKey(priv_bio, NULL, NULL, NULL);
    BIO_free(priv_bio);

    if (!pkey) {
        std::cerr << "Error reading private key." << std::endl;
        return false;
    }

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (!ctx || EVP_PKEY_decrypt_init(ctx) <= 0 || EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        std::cerr << "Error initializing decryption context." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    size_t outlen;
    if (EVP_PKEY_decrypt(ctx, NULL, &outlen, ciphertext.data(), ciphertext.size()) <= 0) {
        std::cerr << "Error calculating decrypted size." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    plaintext.resize(outlen);
    if (EVP_PKEY_decrypt(ctx, plaintext.data(), &outlen, ciphertext.data(), ciphertext.size()) <= 0) {
        std::cerr << "Error during decryption." << std::endl;
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(ctx);
        return false;
    }

    EVP_PKEY_free(pkey);
    EVP_PKEY_CTX_free(ctx);
    
    return true;
}

bool Encryption::CreateFingerprint(const std::string& publicKeyPEM, std::vector<unsigned char>& fingerprint)
{
    EVP_PKEY* publicKey = nullptr;
    BIO* pub_bio = nullptr;
    unsigned char hash[SHA256_DIGEST_LENGTH];
    bool success = false;

    pub_bio = BIO_new_mem_buf(publicKeyPEM.data(), -1);
    if (!pub_bio) {
        std::cerr << "Failed to create BIO for public key" << std::endl;
        return false;
    }

    publicKey = PEM_read_bio_PUBKEY(pub_bio, nullptr, nullptr, nullptr);
    BIO_free(pub_bio);

    if (!publicKey) {
        std::cerr << "Failed to load public key" << std::endl;
        return false;
    }

    unsigned char* pubkey_der = nullptr;
    int pubkey_len = i2d_PUBKEY(publicKey, &pubkey_der);

    if (pubkey_len < 0 || !pubkey_der) {
        std::cerr << "Failed to convert public key to DER format" << std::endl;
    }

    SHA256(pubkey_der, pubkey_len, hash);

    fingerprint.assign(hash, hash + SHA256_DIGEST_LENGTH);
    success = true;

    EVP_PKEY_free(publicKey);
    OPENSSL_free(pubkey_der);

    return success;
}





