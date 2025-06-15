#include <iostream>
#include <vector>
#include <cassert>
#include <memory>
#include <crypto/AuthCrypto.hpp>
#include <crypto/Hash.hpp>
#include <crypto/StringEncoder.hpp>
#include <util/logging/Log.hpp>
#include "config.h"

static std::string originalText = R"(The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. 12345678890123456788901234567890)";

void testEncryptionDecryption() {
    std::unique_ptr<AuthCrypto> authCrypto = std::make_unique<AuthCrypto>(BACKEND_RSA_FILE_NAME);
    // authCrypto->generateKeyPair("");
    if (!authCrypto->doesPublicKeyExists()) {
        std::cerr << "Public key does not exist. Generating new key pair." << std::endl;
        return;
    } else {
        std::cout << "Public key exists." << std::endl;
        authCrypto->loadPublicKey();
    }
    if (!authCrypto->doesPrivateKeyExists()) {
        std::cerr << "Private key does not exist. Generating new key pair." << std::endl;
        return;
    } else {
        std::cout << "Private key exists." << std::endl;
        authCrypto->loadPrivateKey("6102e3b0");
    }

    std::vector<byte> data = StringEncoder::stringToBytes(originalText);
    byte* encryptedData = nullptr;
    byte* decryptedData = nullptr;

    std::cout << "Original length: " << originalText.size() << std::endl;
    size_t encryptedSize = authCrypto->encrypt(data.data(), encryptedData);
    size_t decryptedSize = authCrypto->decrypt(encryptedData, encryptedSize, decryptedData);

    std::string decryptedText = StringEncoder::bytesToString(decryptedData, decryptedSize);

    std::cout << "Original text: " << originalText << std::endl;
    std::cout << "Encrypted text (hex): " << StringEncoder::bytesToHex(encryptedData, encryptedSize) << std::endl;

    std::cout << "Encrypted text (base64): " << StringEncoder::base64Encode(encryptedData, encryptedSize) << std::endl;
    std::cout << "Decrypted text: " << decryptedText << std::endl;

    std::cout << ((originalText == decryptedText)?"TRUE":"FALSE") << std::endl;
    assert(originalText == decryptedText && "Decrypted text matches the original text");
    
    std::cout << "Test passed: Decrypted text matches the original text." << std::endl;

    // Clean up
    delete[] encryptedData;
    delete[] decryptedData;
}

void testHashing() {
    std::vector<byte> data = StringEncoder::stringToBytes(originalText);
    byte* md5Hash = Hash::md5(data.data());
    byte* sha1Hash = Hash::sha1(data.data());
    byte* sha256Hash = Hash::sha256(data.data());

    std::string md5Hex = StringEncoder::bytesToHex(md5Hash, EVP_MAX_MD_SIZE);
    std::string sha1Hex = StringEncoder::bytesToHex(sha1Hash, SHA_DIGEST_LENGTH);
    std::string sha256Hex = StringEncoder::bytesToHex(sha256Hash, SHA256_DIGEST_LENGTH);

    unsigned long long md5Int = StringEncoder::hexToInteger(md5Hex);
    unsigned long long sha1Int = StringEncoder::hexToInteger(sha1Hex);
    unsigned long long sha256Int = StringEncoder::hexToInteger(sha256Hex);

    std::cout << "MD5 hash (Hex): " << md5Hex << std::endl;
    std::cout << "SHA1 hash (Hex): " << sha1Hex << std::endl;
    std::cout << "SHA256 hash (Hex): " << sha256Hex << std::endl;

    std::cout << "MD5 hash (Integer): " << md5Int << std::endl;
    std::cout << "SHA1 hash (Integer): " << sha1Int << std::endl;
    std::cout << "SHA256 hash (Integer): " << sha256Int << std::endl;

    assert("fd684e0acf69f9b0ba6ef594973b2c97000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000" 
        == md5Hex && "Md5 Hash value matches");
    assert("44c8b2d558fb360aa9a8b41ec4ed33677e4f002b" 
        == sha1Hex && "SHA1 Hash value matches");
    assert("276b5acc956324376962a341cb0b341d4ae1b093f2f4b823b093587da31845a5" 
        == sha256Hex && "SHA256 Hash value matches");

    std::string hashValueStr = "18446744073709551615";
    unsigned long long hashValue = std::stoul(hashValueStr, nullptr, 10);
    assert(hashValue == md5Int);
    assert(hashValue == sha1Int);
    assert(hashValue == sha256Int);
}

int main(int argc, char* argv[]) {
    testEncryptionDecryption();
    testHashing();
    return EXIT_SUCCESS;
}