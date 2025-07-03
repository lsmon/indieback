#include <crypto/AuthCrypto.hpp>
#include "Exception.hpp"
#include "util/String.hpp"
#include "util/Util.hpp"
#include "util/LogUtil.hpp"
#include "util/logging/Log.hpp"
#include <filesystem>
#include <cstring>
#include "config.h"

AuthCrypto::AuthCrypto() : filename(DEFAULT_KEY_FILE_NAME)
{
    init();
}

AuthCrypto::AuthCrypto(const char *filename) : filename(filename)
{
    init();
}

bool AuthCrypto::doesPrivateKeyExists()
{
    std::ifstream f(this->private_key_file.c_str());
    return f.good();
}

bool AuthCrypto::doesPublicKeyExists()
{
    std::ifstream f(this->public_key_file.c_str());
    return f.good();
}

AuthCrypto::~AuthCrypto()
{
    destroy();
}

void AuthCrypto::destroy()
{
    try
    {
        unloadPublicKey();
        unloadPrivateKey();
        if (ctx != nullptr)
        {
            EVP_PKEY_CTX_free(ctx);
            ctx = nullptr;
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error during destruction: " << e.what();
    }
}

void AuthCrypto::unloadPublicKey()
{
    if (public_key != nullptr)
    {
        EVP_PKEY_free(public_key);
        this->public_key = nullptr;
    }
}

void AuthCrypto::unloadPrivateKey()
{
    if (private_key != nullptr)
    {
        EVP_PKEY_free(private_key);
        this->private_key = nullptr;
    }
}
bool AuthCrypto::generatePublicKey(EVP_PKEY *pkey)
{
    if (pkey == nullptr)
    {

        std::cerr << "rsa key creation has failed" << std::endl;
        return false;
    }
    BIO *bio = BIO_new(BIO_s_file());
    if (bio == nullptr)
    {
        destroy();
        std::cerr << "public key bio generation failed" << std::endl;
        return false;
    }
    if (BIO_write_filename(bio, (void *)public_key_file.c_str()) != 1)
    {
        destroy();
        std::cerr << "public key file creation failed" << std::endl;
        return false;
    }
    if (PEM_write_bio_PUBKEY(bio, pkey) != 1)
    {
        destroy();
        std::cerr << "public key writing failed" << std::endl;
        return false;
    }
    BIO_free_all(bio);
    return true;
}

bool AuthCrypto::generatePrivateKey(EVP_PKEY *pkey, const char *password)
{
    BIO *bio = BIO_new_file(this->private_key_file.c_str(), "w+");
    if (bio == nullptr)
    {
        destroy();
        std::cerr << "private key bio generation failed" << std::endl;
        return false;
    }
    size_t password_len = (password == nullptr) ? 0 : strlen(password);
    if (PEM_write_bio_PKCS8PrivateKey(bio, pkey, nullptr, password, password_len, nullptr, nullptr) != 1)
    {
        destroy();
        std::cerr << "private key writing failed" << std::endl;
        return false;
    }
    BIO_free_all(bio);
    return true;
}

bool AuthCrypto::generateKeyPair(const char *password)
{
    EVP_PKEY *pkey = EVP_RSA_gen(RSA_KEY_SIZE);
    bool result = generatePublicKey(pkey) && generatePrivateKey(pkey, password);
    if (pkey != nullptr)
    {
        EVP_PKEY_free(pkey);
    }
    else
    {
        throw std::runtime_error("nullptr exception");
    }
    return result;
}

void AuthCrypto::init()
{
    if (this->filename == nullptr)
        throw std::invalid_argument("File name is NULL");
    if (!std::filesystem::exists(PUB_KEY_ARCHIVE))
        std::filesystem::create_directories(PUB_KEY_ARCHIVE);
    if (!std::filesystem::exists(PRV_KEY_ARCHIVE))
        std::filesystem::create_directories(PRV_KEY_ARCHIVE);
    this->public_key_file = PUB_KEY_ARCHIVE;
    this->public_key_file.append("/");
    this->public_key_file.append(this->filename);
    this->public_key_file.append(PUB_KEY_POSTFIX);

    this->private_key_file = PRV_KEY_ARCHIVE;
    this->private_key_file.append("/");
    this->private_key_file.append(this->filename);
    this->private_key_file.append(PRV_KEY_POSTFIX);
}

bool AuthCrypto::loadPrivateKey(const char *password)
{
    FILE *fp;
    fp = fopen(private_key_file.c_str(), "r");
    if (fp == nullptr)
    {
        std::cerr << "private key read has failed" << std::endl;
        return false;
    }
    this->private_key = PEM_read_PrivateKey(fp, nullptr, nullptr, (void *)password);
    fclose(fp);
    if (!this->private_key)
    {
        return false;
    }
    return true;
}

bool AuthCrypto::loadPublicKey()
{
    FILE *fp;
    fp = fopen(public_key_file.c_str(), "r");
    if (fp == nullptr)
    {
        std::cerr << "public key read has failed" << std::endl;
        return false;
    }
    this->public_key = PEM_read_PUBKEY(fp, nullptr, nullptr, nullptr);
    fclose(fp);
    if (!this->public_key)
    {
        return false;
    }
    return true;
}

size_t AuthCrypto::encrypt(unsigned char *src, unsigned char *&out)
{
    if (public_key == nullptr)
        if (!loadPublicKey())
            return -1;
    if (ctx == nullptr)
        ctx = EVP_PKEY_CTX_new(public_key, nullptr);

    if (EVP_PKEY_encrypt_init(ctx) <= 0)
    {
        std::cerr << "encryption init failed" << std::endl;
        destroy();
        return -1;
    }
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
    {
        destroy();
        std::cerr << "padding failed" << std::endl;
        return -1;
    }
    size_t out_len;
    size_t src_len = std::strlen((char *)src);
    if (EVP_PKEY_encrypt(ctx, nullptr, &out_len, src, src_len) <= 0)
    {
        std::cerr << "encryption length computation error" << std::endl;
        destroy();
        return -1;
    }
    out = (unsigned char *)OPENSSL_zalloc(out_len);
    if (EVP_PKEY_encrypt(ctx, out, &out_len, src, src_len) <= 0)
    {
        std::cerr << "encryption error" << std::endl;
        destroy();
        return -1;
    }
    destroy();
    return out_len;
}

size_t AuthCrypto::decrypt(unsigned char *src, size_t src_len, unsigned char *&out, const char *password)
{
    if (private_key == nullptr)
        if (!loadPrivateKey(password))
            return -1;
    if (ctx == nullptr)
        ctx = EVP_PKEY_CTX_new(private_key, nullptr);

    if (EVP_PKEY_decrypt_init(ctx) <= 0)
    {
        std::cerr << "decryption init error" << std::endl;
        destroy();
        return -1;
    }
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
    {
        destroy();
        std::cerr << "padding failed" << std::endl;
        return -1;
    }
    size_t out_len;
    if (EVP_PKEY_decrypt(ctx, nullptr, &out_len, src, src_len) <= 0)
    {
        std::cerr << "decryption length computation error" << std::endl;
        destroy();
        return -1;
    }
    out = (unsigned char *)OPENSSL_zalloc(out_len);
    if (EVP_PKEY_decrypt(ctx, out, &out_len, src, src_len) <= 0)
    {
        std::cerr << "decryption init error" << std::endl;
        destroy();
        return -1;
    }
    destroy();
    return out_len;
}

bool AuthCrypto::isPublicKeyRsa()
{
    if (public_key == nullptr)
        if (!loadPublicKey())
            return false;
    std::cout << EVP_PKEY_get0_type_name(public_key) << std::endl;
    return (EVP_PKEY_get_base_id(public_key) == EVP_PKEY_RSA);
}

size_t AuthCrypto::hashing(unsigned char *msg, unsigned char *&md_value)
{
    md_value = static_cast<unsigned char *>(malloc(EVP_MAX_MD_SIZE));

    const EVP_MD *md = EVP_MD_fetch(nullptr, RSA_HASH_ALGO, nullptr);
    if (md == nullptr)
    {
        std::cerr << "Unsupported algorithm: " << RSA_HASH_ALGO << std::endl;
        destroy();
        return -1;
    }
    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (mdctx == nullptr)
    {
        std::cerr << "Message Digestion context initialization failed" << std::endl;
        destroy();
        return -1;
    }
    EVP_DigestInit_ex2(mdctx, md, nullptr);
    EVP_DigestUpdate(mdctx, msg, strlen((char *)msg));
    unsigned int md_length = 0;
    EVP_DigestFinal(mdctx, md_value, &md_length);
    EVP_MD_CTX_free(mdctx);
    return md_length;
}

size_t AuthCrypto::sign(const char *msg, unsigned char *&sig, const char *password)
{
    if (private_key == nullptr)
    {
        if (!loadPrivateKey(password))
            return -1;
    }

    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    if (md_ctx == nullptr)
    {
        unloadPrivateKey();
        std::cerr << "Message Digest init failed." << std::endl;
        return -1;
    }
    OSSL_LIB_CTX *lib_ctx = OSSL_LIB_CTX_new();
    if (EVP_DigestSignInit_ex(md_ctx, nullptr, RSA_HASH_ALGO, lib_ctx, nullptr, private_key, nullptr) < 1)
    {
        unloadPrivateKey();
        std::cerr << "Message Digest Signing init failed." << std::endl;
        return -1;
    }
    if (EVP_DigestUpdate(md_ctx, msg, strlen(msg)) < 1)
    {
        unloadPrivateKey();
        std::cerr << "Message Digest Signing Update failed." << std::endl;
        return -1;
    }
    size_t sig_len;
    if (EVP_DigestSignFinal(md_ctx, nullptr, &sig_len) < 1)
    {
        unloadPrivateKey();
        std::cerr << "Signing Length Computation failed." << std::endl;
        return -1;
    }
    if (sig_len <= 0)
    {
        unloadPrivateKey();
        std::cerr << "Signing Length invalid: " << sig_len << std::endl;
        return -1;
    }
    sig = (unsigned char *)OPENSSL_malloc(sig_len);
    if (sig == nullptr)
    {
        unloadPrivateKey();
        std::cerr << "Signing allocation failed." << std::endl;
        return -1;
    }
    if (EVP_DigestSignFinal(md_ctx, sig, &sig_len) < 1)
    {
        unloadPrivateKey();
        std::cerr << "Signing failed." << std::endl;
        OPENSSL_free(sig);
        return -1;
    }
    EVP_MD_CTX_free(md_ctx);
    unloadPrivateKey();
    return sig_len;
}

bool AuthCrypto::verify(const char *msg, unsigned char *sig, size_t sig_len)
{
    if (public_key == nullptr)
        if (!loadPublicKey())
            return false;

    EVP_MD_CTX *md_ctx = EVP_MD_CTX_new();
    if (md_ctx == nullptr)
    {
        unloadPublicKey();
        std::cerr << "Message Digest init failed." << std::endl;
        return false;
    }

    OSSL_LIB_CTX *lib_ctx = OSSL_LIB_CTX_new();
    if (EVP_DigestVerifyInit_ex(md_ctx, nullptr, RSA_HASH_ALGO, lib_ctx, nullptr, public_key, nullptr) < 1)
    {
        unloadPublicKey();
        std::cerr << "Message Digest Signing init failed." << std::endl;
        return false;
    }

    if (EVP_DigestVerifyUpdate(md_ctx, msg, strlen(msg)) < 1)
    {
        unloadPublicKey();
        std::cerr << "Message Digest Signing Update failed." << std::endl;
        return false;
    }

    if (EVP_DigestVerifyFinal(md_ctx, sig, sig_len) < 1)
    {
        unloadPublicKey();
        std::cerr << "Verifying failed." << std::endl;
        return false;
    }
    EVP_MD_CTX_free(md_ctx);
    unloadPublicKey();
    return true;
}

std::string AuthCrypto::getPublicKeyFilename()
{
    return public_key_file;
}

std::string AuthCrypto::getPrivateKeyFilename()
{
    return private_key_file;
}

void AuthCrypto::loadPublicKey(std::string filename)
{
    this->public_key_file = filename;
    this->loadPublicKey();
}

void AuthCrypto::loadPrivateKey(std::string filename, const char *passphrase)
{
    this->private_key_file = filename;
    this->loadPrivateKey(passphrase);
}