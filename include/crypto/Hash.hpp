#ifndef AUTH_HASH_HPP
#define AUTH_HASH_HPP

#include "config.h"

class Hash {
public:
    static byte* md5(const byte* text);

    static byte* sha1(const byte* text);

    static byte* sha256(const byte* text);

};

#endif // AUTH_HASH_HPP