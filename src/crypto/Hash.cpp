#include "crypto/Hash.hpp"
#include <openssl/evp.h>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include <cstring>

byte* Hash::md5(const byte* text) 
{
    // MD5(text, strlen((char*)text), hash);
    static byte hash[EVP_MAX_MD_SIZE];
    unsigned int hash_len;

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
        if (mdctx == NULL) {
            return nullptr; // Error handling for memory allocation
        }

        if (1 != EVP_DigestInit_ex(mdctx, EVP_md5(), NULL)) {
            EVP_MD_CTX_free(mdctx);
            return nullptr; // Error initializing digest context
        }

        if (1 != EVP_DigestUpdate(mdctx, text, strlen((char*)text))) {
            EVP_MD_CTX_free(mdctx);
            return nullptr; // Error updating digest
        }

        if (1 != EVP_DigestFinal_ex(mdctx, hash, &hash_len)) {
            EVP_MD_CTX_free(mdctx);
            return nullptr; // Error finalizing digest
        }

        EVP_MD_CTX_free(mdctx); // Free the context
        return hash;
    return hash;
}


byte *Hash::sha1(const byte *text)
{
    static byte hash[SHA_DIGEST_LENGTH];
    SHA1(text, strlen((char*)text), hash);
    return hash;
}

byte *Hash::sha256(const byte *text)
{
    static byte hash[SHA256_DIGEST_LENGTH];
    SHA256(text, strlen((char*)text), hash);
    return hash;
}
