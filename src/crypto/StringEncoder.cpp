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

std::vector<byte> StringEncoder::base64Decode(const std::string &encodedData)
{
    BIO *bio, *b64;
    int decodeLen = encodedData.length();
    std::vector<byte> decodedData(decodeLen);

    b64 = BIO_new(BIO_f_base64());
    bio = BIO_new_mem_buf(encodedData.data(), decodeLen);
    bio = BIO_push(b64, bio);

    decodeLen = BIO_read(bio, decodedData.data(), decodeLen);
    decodedData.resize(decodeLen);

    BIO_free_all(bio);

    return decodedData;
}

std::string StringEncoder::bytesToHex(const byte *bytes, size_t length)
{
    std::ostringstream oss;
    for (size_t i = 0; i < length; ++i) {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(bytes[i]);
    }
    return oss.str();
}

unsigned long long StringEncoder::hexToInteger(const std::string &hexStr)
{
    unsigned long long value;
    std::istringstream iss(hexStr);
    iss >> std::hex >> value;
    return value;
}
