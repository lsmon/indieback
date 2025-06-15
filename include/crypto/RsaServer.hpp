#ifndef INDIEPUB_RSA_SERVER_HPP
#define INDIEPUB_RSA_SERVER_HPP

#include <crypto/AuthCrypto.hpp>
#include <memory>

class RsaServer {
public:    
    static std::unique_ptr<AuthCrypto> getInstance();
};

#endif // INDIEPUB_RSA_SERVER_HPP