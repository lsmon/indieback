#include <backend/models/User.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::User::COLUMN_FAMILY = "users";
const std::string indiepub::User::IDX_USERS_EMAIL = "email";
const std::string indiepub::User::IDX_USERS_ROLE = "role";
const std::string indiepub::User::IDX_USERS_NAME = "name";

indiepub::User::User(const std::string &user_id, const std::string &email, const std::string &role,
                     const std::string &name, std::time_t created_at)
    : user_id_(user_id), email_(email), role_(role), name_(name), created_at_(created_at) {}

std::string indiepub::User::user_id() const
{
    return user_id_;
}

std::string indiepub::User::email() const
{
    return email_;
}

std::string indiepub::User::role() const
{
    return role_;
}

std::string indiepub::User::name() const
{
    return name_;
}

std::time_t indiepub::User::created_at() const
{
    return created_at_;
}

std::string indiepub::User::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("user_id", user_id_);
    json->put("email", email_);
    json->put("role", role_);
    json->put("name", name_);
    json->put("created_at", indiepub::timestamp_to_string(created_at_));
    return json->str();
}

indiepub::User indiepub::User::from_json(const std::string &json)
{
    User user;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    user.user_id_ = jsonObject->get("user_id").str();
    user.email_ = jsonObject->get("email").str();
    user.role_ = jsonObject->get("role").str();
    user.name_ = jsonObject->get("name").str();
    user.created_at_ = indiepub::string_to_timestamp(jsonObject->get("created_at").str());
    return user;
}

indiepub::User indiepub::User::from_row(const CassRow *row)
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

        // Extract email (TEXT)
        const char *email;
        size_t email_length;
        const CassValue *email_value = cass_row_get_column_by_name(row, "email");
        if (cass_value_get_string(email_value, &email, &email_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get email");
        }

        // Extract role (TEXT)
        const char *role;
        size_t role_length;
        const CassValue *role_value = cass_row_get_column_by_name(row, "role");
        if (cass_value_get_string(role_value, &role, &role_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get role");
        }

        // Extract name (TEXT)
        const char *name;
        size_t name_length;
        const CassValue *name_value = cass_row_get_column_by_name(row, "name");
        if (cass_value_get_string(name_value, &name, &name_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get name");
        }

        // Extract created_at (TIMESTAMP)
        cass_int64_t created_at;
        const CassValue *created_at_value = cass_row_get_column_by_name(row, "created_at");
        if (cass_value_get_int64(created_at_value, &created_at) != CASS_OK)
        {
            throw std::runtime_error("Failed to get created_at");
        }

        // Construct and return the User object
        return User(
            std::string(user_id_str),
            std::string(email, email_length),
            std::string(role, role_length),
            std::string(name, name_length),
            static_cast<std::time_t>(created_at / 1000) // Convert from milliseconds to seconds
        );
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return User();
    }
}
