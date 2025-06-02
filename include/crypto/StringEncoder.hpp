#ifndef __STRING_ENCODER_HPP__
#define __STRING_ENCODER_HPP__

#include "config.h"
#include <string>
#include <vector>

class StringEncoder {
public:
    static std::vector<byte> stringToBytes(const std::string& str);

    static std::string bytesToString(const byte* bytes, size_t length);

    static std::string base64Encode(const byte* bytes, size_t length);

    static std::vector<byte> base64Decode(const std::string& encodedData);

    static std::string bytesToHex(const byte* bytes, size_t length);

    static unsigned long long hexToInteger(const std::string& hexStr);
};

#endif // __STRING_ENCODER_HPP__