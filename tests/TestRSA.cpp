#include <iostream>
#include <vector>
#include <cassert>
#include <memory>
#include <crypto/AuthCrypto.hpp>
#include <crypto/Hash.hpp>
#include <crypto/StringEncoder.hpp>
#include <util/logging/Log.hpp>
#include "config.h"

static std::string originalText = R"(The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. The quick brown fox jumps over the lazy dog, showcasing a classic pangram used for typing practice and font display. ABCDEFGHIJKLMNOPQRSTUVWXYZ123456)";
std::unique_ptr<AuthCrypto> authCrypto = std::make_unique<AuthCrypto>("indieback");

std::string testEncryption() {
     if (!authCrypto->doesPrivateKeyExists()) {
        std::cerr << "Public key does not exist. Generating new key pair." << std::endl;
        return "";
    } else {
        std::cout << "Public key exists." << std::endl;
        authCrypto->loadPrivateKey("");
    }
    
    std::vector<byte> data = StringEncoder::stringToBytes(originalText);
    byte* encryptedData = nullptr;

    std::cout << "Original length: " << originalText.size() << std::endl;
    size_t encryptedSize = authCrypto->encrypt(data.data(), encryptedData);

    std::string encryptedText = StringEncoder::base64Encode(encryptedData, encryptedSize);
    delete [] encryptedData;
    return encryptedText;
}

std::string testDecryption(std::string encryptedText) 
{
    if (!authCrypto->doesPublicKeyExists()) {
        std::cerr << "Private key does not exist. Generating new key pair." << std::endl;
        return "";
    } else {
        std::cout << "Private key exists." << std::endl;
        authCrypto->loadPublicKey();
    }

    std::vector<byte> data = StringEncoder::base64Decode(encryptedText);
    byte* decryptedData = nullptr;

    size_t decryptedSize = authCrypto->decrypt(data.data(), data.size(), decryptedData);
    std::string decryptedText = StringEncoder::bytesToString(decryptedData, decryptedSize);

    assert(originalText == decryptedText && "Decrypted text matches the original text");

    // Clean up
    delete[] decryptedData;
    return decryptedText;
}

std::string signing(std::string text) {
    if (!authCrypto->doesPrivateKeyExists()) {
        std::cerr << "Private key does not exist. Generating new key pair." << std::endl;
        return "";
    } else {
        std::cout << "Private key exists." << std::endl;
        authCrypto->loadPrivateKey("");
    }

    byte* signature = nullptr;
    size_t signature_length = authCrypto->sign(text.c_str(), signature, "");
    std::string signatureHex = StringEncoder::base64Encode(signature, signature_length);
    
    // Clean up
    delete[] signature;

    return signatureHex;
}

bool verifySignature(std::string text, std::string signature) {
    if (!authCrypto->doesPublicKeyExists()) {
        std::cerr << "Public key does not exist. Generating new key pair." << std::endl;
        return false;
    } else {
        std::cout << "Public key exists." << std::endl;
        authCrypto->loadPublicKey();
    }

    std::vector<byte> signatureBytes = StringEncoder::base64Decode(signature);
    bool isVerified = authCrypto->verify(text.c_str(), signatureBytes.data(), signatureBytes.size());
    
    return isVerified;
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
    // testEncryptionDecryption();
    // testHashing();
    std::cout << "Original text: " << originalText << std::endl;
    
    std::cout << "Testing RSA encryption and decryption..." << std::endl;
    
    std::string encryptedText = testEncryption();
    std::cout << "Encrypted text: " << encryptedText << std::endl;
    std::string decryptedText = testDecryption(encryptedText);
    std::cout << "Decrypted text: " << decryptedText << std::endl;

    assert(originalText == decryptedText && "Decrypted text matches the original text");

    std::cout << "Testing RSA signing and verification..." << std::endl;
    std::string signatureHex = signing(originalText);
    std::cout << "Signature: " << signatureHex << std::endl;
    bool isVerified = verifySignature(originalText, signatureHex);
    std::cout << "Signature verification: " << (isVerified ? "Success" : "Failure") << std::endl;
    assert(isVerified && "Signature verification succeeded");

    return EXIT_SUCCESS;
}