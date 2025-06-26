#include "crypto/StringEncoder.hpp"
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <openssl/bio.h>

std::vector<byte> StringEncoder::stringToBytes(const std::string& str) {
    return std::vector<unsigned char>(str.begin(), str.end());
}

std::string StringEncoder::bytesToString(const byte *bytes, size_t length)
{
    return std::string(reinterpret_cast<const char*>(bytes), length);
}

std::string StringEncoder::base64Encode(const byte *bytes, size_t length)
{
    BIO *bio, *b64;
    BUF_MEM *bufferPtr;
    
    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, bytes, length);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string encodedData(bufferPtr->data, bufferPtr->length);
    BUF_MEM_free(bufferPtr);

    return encodedData;
}

std::vector<byte> StringEncoder::base64Decode(const std::string &encoded)
{
    BIO* bio = BIO_new_mem_buf(encoded.c_str(), encoded.size());
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL); // Handle base64 without newlines
    bio = BIO_push(b64, bio);
    std::vector<unsigned char> decoded((encoded.size() * 3) / 4); // Approximate size
    int len = BIO_read(bio, decoded.data(), decoded.size());
    BIO_free_all(bio);
    if (len > 0) {
        decoded.resize(len); // Adjust to actual size
    } else {
        decoded.clear(); // Decoding failed
    }
    return decoded;
}

std::string StringEncoder::bytesToHex(const byte *bytes, size_t length)
{
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

std::vector<byte> StringEncoder::hexToBytes(const std::string &hexStr)
{
    std::vector<byte> bytes;
    size_t len = hexStr.length();
    bytes.reserve(len / 2);
    
    for (size_t i = 0; i < len; i += 2) {
        std::string byteStr = hexStr.substr(i, 2);
        unsigned char byte = static_cast<unsigned char>(std::stoul(byteStr, nullptr, 16));
        bytes.push_back(byte);
    }
    
    return bytes;
}

unsigned long long StringEncoder::hexToInteger(const std::string &hexStr)
{
    unsigned long long value;
    std::istringstream iss(hexStr);
    iss >> std::hex >> value;
    return value;
}
