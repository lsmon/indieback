#ifndef INDIEPUB_RSA_OPERATIONS_HPP
#define INDIEPUB_RSA_OPERATIONS_HPP

#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <string>
#include <stdexcept>
#include <memory>
#include <iostream>
#include <util/Util.hpp>

class RSAOperations {
private:
    RSA *rsa;
    void handleErrors() {
        unsigned long errCode = ERR_get_error();
        if (errCode) {
            char errMsg[120];
            ERR_error_string_n(errCode, errMsg, sizeof(errMsg));
            throw std::runtime_error(std::string("RSA error: ") + errMsg);
        }
    }
public:
    RSAOperations() : rsa(nullptr) {
        rsa = RSA_new();
        if (!rsa) {
            handleErrors();
        }
    }

    ~RSAOperations() {
        if (rsa) {
            RSA_free(rsa);
        }
    }

    void generateKeyPair(int bits = 2048) {
        BIGNUM *bn = BN_new();
        if (!bn) {
            handleErrors();
        }
        if (BN_set_word(bn, RSA_F4) != 1) {
            BN_free(bn);
            handleErrors();
        }
        if (RSA_generate_key_ex(rsa, bits, bn, nullptr) != 1) {
            BN_free(bn);
            handleErrors();
        }
        BN_free(bn);
    }

    std::string publicKeyToPEM() const {
        BIO *bio = BIO_new(BIO_s_mem());
        if (!bio || !PEM_write_bio_RSA_PUBKEY(bio, rsa)) {
            BIO_free(bio);
            handleErrors();
        }
        BUF_MEM *buf;
        BIO_get_mem_ptr(bio, &buf);
        std::string pem(buf->data, buf->length);
        BIO_free(bio);
        return pem;
    }

    std::string privateKeyToPEM() const {
        BIO *bio = BIO_new(BIO_s_mem());
        if (!bio || !PEM_write_bio_RSAPrivateKey(bio, rsa, nullptr, nullptr, 0, nullptr, nullptr)) {
            BIO_free(bio);
            handleErrors();
        }
        BUF_MEM *buf;
        BIO_get_mem_ptr(bio, &buf);
        std::string pem(buf->data, buf->length);
        BIO_free(bio);
        return pem;
    }

    // Additional methods for encryption/decryption can be added here
    std::string encrypt(const std::string &data) {
        std::unique_ptr<unsigned char[]> encrypted(new unsigned char[RSA_size(rsa)]);
        int result = RSA_public_encrypt(data.size(), reinterpret_cast<const unsigned char*>(data.c_str()), encrypted.get(), rsa, RSA_PKCS1_OAEP_PADDING);
        if (result == -1) {
            handleErrors();
        }
        return std::string(reinterpret_cast<char*>(encrypted.get()), result);
    }

    std::string decrypt(const std::string &encryptedData) {
        std::unique_ptr<unsigned char[]> decrypted(new unsigned char[RSA_size(rsa)]);
        int result = RSA_private_decrypt(encryptedData.size(), reinterpret_cast<const unsigned char*>(encryptedData.c_str()), decrypted.get(), rsa, RSA_PKCS1_OAEP_PADDING);
        if (result == -1) {
            handleErrors();
        }
        return std::string(reinterpret_cast<char*>(decrypted.get()), result);
    }

    std::string sign(const std::string &data) {
        std::unique_ptr<unsigned char[]> signature(new unsigned char[RSA_size(rsa)]);
        unsigned int sigLen;
        if (RSA_sign(NID_sha256, reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), signature.get(), &sigLen, rsa) != 1) {
            handleErrors();
        }
        return std::string(reinterpret_cast<char*>(signature.get()), sigLen);
    }

    bool verify(const std::string &data, const std::string &signature) {
        return RSA_verify(NID_sha256, reinterpret_cast<const unsigned char*>(data.c_str()), data.size(),
                          reinterpret_cast<const unsigned char*>(signature.c_str()), signature.size(), rsa) == 1;
    }
};

#endif // INDIEPUB_RSA_OPERATIONS_HPP