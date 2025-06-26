#ifndef INDIEPUB_CREDENTIALS_CONTROLLER_HPP
#define INDIEPUB_CREDENTIALS_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/Credentials.hpp>

namespace indiepub 
{
    class CredentialsController : public CassandraConnection
    {
    private:
        
    public:
        CredentialsController() = default;
        CredentialsController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);
        
        bool insertCredentials(const indiepub::Credentials &creds);
        indiepub::Credentials getCredentialsByUserId(const std::string &user_id);
        indiepub::Credentials getCredentialsByAuthToken(const std::string &auth_token);
        indiepub::Credentials getCredentialsByPwHash(const std::string &pw_hash);
    };
}

#endif