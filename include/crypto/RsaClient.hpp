#ifndef INDIEPUB_RSA_CLIENT_HPP
#define INDIEPUB_RSA_CLIENT_HPP

#include <crypto/AuthCrypto.hpp>
#include <memory>

class RsaClient {
public:    
    static std::unique_ptr<AuthCrypto> getInstance();
};

#endif // INDIEPUB_RSA_CLIENT_HPP