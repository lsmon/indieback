#ifndef INDIEPUB_CREDENTIALS_HPP
#define INDIEPUB_CREDENTIALS_HPP

#include <string>
#include <cassandra.h>

namespace indiepub
{
    class Credentials
    {

    public:
        Credentials() = default;
        Credentials(const std::string &user_id, const std::string &token, const std::string &pw_hash);

        static const std::string COLUMN_FAMILY;
        static const std::string PK_CREDENTIAL_ID;
        static const std::string IDX_CREDENTIAL_AUTH_TOKEN;
        static const std::string IDX_CREDENTIAL_PW_HASH;

        // Getters
        std::string user_id() const;
        std::string auth_token() const;
        std::string pw_hash() const;

        //JSON serialization
        std::string to_json() const;

        static Credentials from_json(const std::string &json);
        static Credentials from_row(const CassRow *row);
    
    private:
        std::string user_id_;
        std::string auth_token_;
        std::string pw_hash_;
    };
}
#endif