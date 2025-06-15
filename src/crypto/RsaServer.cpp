#include <crypto/RsaServer.hpp>
#include "config.h"

std::unique_ptr<AuthCrypto> RsaServer::getInstance()
{
    std::unique_ptr<AuthCrypto> instance = std::make_unique<AuthCrypto>(BACKEND_RSA_FILE_NAME);
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
