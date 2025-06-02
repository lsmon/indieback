#ifndef __AUTH_CRYPTO_HPP__
#define __AUTH_CRYPTO_HPP__

#include <openssl/bn.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <string>

class AuthCrypto {
private:
    const char *filename;
    std::string public_key_file;
    std::string private_key_file;
    EVP_PKEY *public_key;
    EVP_PKEY *private_key;
    EVP_PKEY_CTX *ctx;

    void init();

    void destroy();

public:
    AuthCrypto();
    
    // Constructors, destructors if needed
    AuthCrypto(const char *filename);

    ~AuthCrypto();

    std::string getPublicKeyFilename();

    std::string getPrivateKeyFilename();

    bool loadPrivateKey(const char *password);

    bool loadPublicKey();
    
    bool doesPrivateKeyExists();

    bool doesPublicKeyExists();

    bool generatePublicKey(EVP_PKEY *pkey);

    bool generatePrivateKey(EVP_PKEY *pkey, const char *password=NULL);

    bool generateKeyPair(const char *password=NULL);

    bool isPublicKeyRsa();

    size_t encrypt(unsigned char *src, unsigned char *&out);

    size_t decrypt(unsigned char *src, size_t src_len, unsigned char *&out, const char *password=NULL);

    size_t sign(const char *msg, unsigned char *&sig, const char *password=NULL);

    bool verify(const char *msg, unsigned char *sig, size_t sig_len);

    size_t hashing(unsigned char *msg, unsigned char *&md);

    void loadPublicKey(std::string filename);

    void loadPrivateKey(std::string filename, const char *password="");
};

#endif // __AUTH_CRYPTO_HPP__