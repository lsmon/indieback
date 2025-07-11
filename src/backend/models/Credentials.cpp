#include <backend/models/Credentials.hpp>
#include <memory>
#include <JSON.hpp>

const std::string indiepub::Credentials::COLUMN_FAMILY = "credentials";
const std::string indiepub::Credentials::PK_CREDENTIAL_ID = "user_id";
const std::string indiepub::Credentials::IDX_CREDENTIAL_AUTH_TOKEN = "auth_token";
const std::string indiepub::Credentials::IDX_CREDENTIAL_PW_HASH = "pw_hash";

indiepub::Credentials::Credentials(const std::string &user_id, const std::string &token, const std::string &pw_hash)
    : user_id_(user_id), auth_token_(token), pw_hash_(pw_hash)
{
}

std::string indiepub::Credentials::user_id() const
{
    return user_id_;
}

std::string indiepub::Credentials::auth_token() const
{
    return auth_token_;
}

std::string indiepub::Credentials::pw_hash() const
{
    return pw_hash_;
}

void indiepub::Credentials::set_user_id(const std::string &user_id)
{
    user_id_ = user_id;
}

void indiepub::Credentials::set_auth_token(const std::string &auth_token)
{
    auth_token_ = auth_token;
}

void indiepub::Credentials::set_pw_hash(const std::string &pw_hash)
{
    pw_hash_ = pw_hash;
}

std::string indiepub::Credentials::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("user_id", user_id_);
    json->put("auth_token", auth_token_);
    json->put("pw_hash", pw_hash_);
    return json->str();
}

indiepub::Credentials indiepub::Credentials::from_json(const std::string &json)
{
    Credentials creds;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    creds.user_id_ = jsonObject->get("user_id").str();
    creds.auth_token_ = jsonObject->get("auth_token").str();
    creds.pw_hash_ = jsonObject->get("pw_hash").str();
    return creds;
}

indiepub::Credentials indiepub::Credentials::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
            throw std::runtime_error("Row is null");
        }

        // Extract user_id (UUID)
        CassUuid user_id;
        const CassValue *user_id_value = cass_row_get_column_by_name(row, "user_id");
        if (cass_value_get_uuid(user_id_value, &user_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get user_id");
        }
        char user_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(user_id, user_id_str);

        // Extract auth_token (TEXT)
        const char *token;
        size_t token_length;
        const CassValue *token_value = cass_row_get_column_by_name(row, "auth_token");
        if (cass_value_get_string(token_value, &token, &token_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get authentication token");
        }

        // Extract pw_hash (TEXT)
        const char *hash;
        size_t hash_length;
        const CassValue *hash_value = cass_row_get_column_by_name(row, "pw_hash");
        if (cass_value_get_string(hash_value, &hash, &hash_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get password hash");
        }

        return indiepub::Credentials(std::string(user_id_str), std::string(token), std::string(hash, hash_length));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return Credentials();
    }
}