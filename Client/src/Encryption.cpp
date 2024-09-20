// // #include "Encryption.h"

// // std::vector<CryptoPP::byte> Encryption::GenerateRandomBytes(size_t length) {

// //     CryptoPP::AutoSeededRandomPool prng;
// //     std::vector<CryptoPP::byte> bytes(length);
// //     prng.GenerateBlock(bytes.data(), length);
    
// //     return bytes;
// // }

// #include <iostream>
// #include <cryptopp/rsa.h>
// #include <cryptopp/pssr.h>
// #include <cryptopp/osrng.h>
// #include <cryptopp/aes.h>
// #include <cryptopp/gcm.h>
// #include <cryptopp/base64.h>
// #include <cryptopp/sha.h>
// #include <cryptopp/filters.h>
// #include <cryptopp/files.h>

// using namespace CryptoPP;
// using namespace std;

// // Helper function to encode to Base64
// string Base64Encode(const string &input) {
//     string encoded;
//     StringSource ss(input, true, new Base64Encoder(new StringSink(encoded)));
//     return encoded;
// }

// // Helper function to decode from Base64
// string Base64Decode(const string &input) {
//     string decoded;
//     StringSource ss(input, true, new Base64Decoder(new StringSink(decoded)));
//     return decoded;
// }

// // Generate RSA key pair (public and private keys)
// void GenerateRSAKeyPair(RSA::PrivateKey &privateKey, RSA::PublicKey &publicKey) {
//     AutoSeededRandomPool rng;
//     privateKey.GenerateRandomWithKeySize(rng, 2048);
//     publicKey.AssignFrom(privateKey);
// }

// // Sign the message using RSA-PSS with SHA-256
// string SignMessage(const RSA::PrivateKey &privateKey, const string &message) {
//     AutoSeededRandomPool rng;
//     RSASS<PSS, SHA256>::Signer signer(privateKey);

//     string signature;
//     StringSource ss(message, true, new SignerFilter(rng, signer, new StringSink(signature)));
//     return signature;
// }

// // Encrypt a message with AES-GCM
// string AESEncrypt(const SecByteBlock &key, const SecByteBlock &iv, const string &plaintext) {
//     string ciphertext;

//     GCM<AES>::Encryption aes;
//     aes.SetKeyWithIV(key, key.size(), iv, iv.size());

//     StringSource ss(plaintext, true,
//         new AuthenticatedEncryptionFilter(aes,
//             new StringSink(ciphertext)
//         )
//     );
//     return ciphertext;
// }

// // Encrypt AES key using recipient's RSA public key with OAEP and SHA-256
// string EncryptAESKeyWithRSA(const RSA::PublicKey &recipientPublicKey, const SecByteBlock &aesKey) {
//     AutoSeededRandomPool rng;
//     string encryptedKey;

//     RSAES<OAEP<SHA256>>::Encryptor encryptor(recipientPublicKey);
//     StringSource ss(aesKey.data(), aesKey.size(), true,
//         new PK_EncryptorFilter(rng, encryptor,
//             new StringSink(encryptedKey)
//         )
//     );
//     return encryptedKey;
// }

// // int main() {
// //     // Step 1: Generate RSA key pair for the sender
// //     RSA::PrivateKey senderPrivateKey;
// //     RSA::PublicKey senderPublicKey;
// //     GenerateRSAKeyPair(senderPrivateKey, senderPublicKey);

// //     // Step 2: Generate RSA key pair for the recipient
// //     RSA::PrivateKey recipientPrivateKey;
// //     RSA::PublicKey recipientPublicKey;
// //     GenerateRSAKeyPair(recipientPrivateKey, recipientPublicKey);

// //     // Step 3: Create a message
// //     string message = "This is a confidential message.";

// //     // Step 4: Sign the message using sender's RSA private key (RSA-PSS)
// //     string signature = SignMessage(senderPrivateKey, message);
// //     cout << "Signature (Base64): " << Base64Encode(signature) << endl;

// //     // Step 5: Encrypt the message with AES-GCM
// //     AutoSeededRandomPool rng;
// //     SecByteBlock aesKey(32);  // AES-256 key
// //     rng.GenerateBlock(aesKey, aesKey.size());

// //     SecByteBlock iv(16);  // AES IV
// //     rng.GenerateBlock(iv, iv.size());

// //     string ciphertext = AESEncrypt(aesKey, iv, message);
// //     cout << "Encrypted message (Base64): " << Base64Encode(ciphertext) << endl;

// //     // Step 6: Encrypt the AES key with recipient's RSA public key
// //     string encryptedAESKey = EncryptAESKeyWithRSA(recipientPublicKey, aesKey);
// //     cout << "Encrypted AES key (Base64): " << Base64Encode(encryptedAESKey) << endl;

// //     // The AES ciphertext, IV, and encrypted AES key would be sent in the message

// //     return 0;
// // }
