#include <crypto/RsaClient.hpp>
#include "config.h"

std::unique_ptr<AuthCrypto> RsaClient::getInstance()
{
    std::unique_ptr<AuthCrypto> instance = std::make_unique<AuthCrypto>(FRONTEND_RSA_FILE_NAME);
    if (instance->doesPublicKeyExists())
    {
        instance->loadPublicKey();
    }
    if (instance->doesPrivateKeyExists())
    {
        instance->loadPrivateKey("");
    }
    return instance;
}
