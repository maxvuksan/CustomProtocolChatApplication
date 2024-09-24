
#pragma once
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <mine/mine.h>

class Encryption {
public:
    Encryption();
    ~Encryption();

    // AES Encryption
    bool AESEncrypt(const std::vector<unsigned char>& plaintext, const std::vector<unsigned char>& key,
                            std::vector<unsigned char>& ciphertext, std::vector<unsigned char>& iv, std::vector<unsigned char>& tag);
    bool AESDecrypt(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key,
                            const std::vector<unsigned char>& iv, const std::vector<unsigned char>& tag,
                            std::vector<unsigned char>& plaintext);

    // RSA Encryption
    bool GenerateRSAKeyPair(std::string& publicKey, std::string& privateKey);
    bool RSAEncrypt(const std::vector<unsigned char>& plaintext, const std::string& publicKey, std::vector<unsigned char>& ciphertext);
    bool RSADecrypt(const std::vector<unsigned char>& ciphertext, const std::string& privateKey, std::vector<unsigned char>& plaintext);

    // RSA Signature
    int SignMessage(RSA* privateKey, const unsigned char* message, size_t messageLen, unsigned char* signature, size_t* signatureLen);
    int VerifySignature(RSA* publicKey, const unsigned char* message, size_t messageLen, const unsigned char* signature, size_t signatureLen);

    // Key generation
    bool GenerateRSAKeyPair(int keySize, std::string& privateKeyFile, std::string& publicKeyPEM);

    // helper functions
    std::vector<unsigned char> StringToVector(const std::string& str);
    std::string VectorToString(const std::vector<unsigned char>& vec);
    
private:
    EVP_CIPHER_CTX* aesEncryptCtx;
    EVP_CIPHER_CTX* aesDecryptCtx;

    void HandleOpenSSLErrors();
};
