#include <backend/models/User.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

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
        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &user_id, &user_id_length);

        const char *email;
        size_t email_length;
        cass_value_get_string(cass_row_get_column(row, 1), &email, &email_length);

        const char *role;
        size_t role_length;
        cass_value_get_string(cass_row_get_column(row, 2), &role, &role_length);

        const char *name;
        size_t name_length;
        cass_value_get_string(cass_row_get_column(row, 3), &name, &name_length);

        cass_int64_t created_at;
        cass_value_get_int64(cass_row_get_column(row, 4), &created_at);
        return User(std::string(user_id, user_id_length),
                    std::string(email, email_length),
                    std::string(role, role_length),
                    std::string(name, name_length),
                    created_at);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return User();
    }
}
