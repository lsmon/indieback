#include <backend/models/User.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>
#include <util/logging/Log.hpp>
#include <util/String.hpp>

const std::string indiepub::User::COLUMN_FAMILY = "users";
const std::string indiepub::User::IDX_USERS_EMAIL = "email";
const std::string indiepub::User::IDX_USERS_ROLE = "role";
const std::string indiepub::User::IDX_USERS_NAME = "name";

indiepub::User::User(const std::string &user_id, const std::string &email, const std::string &role,
                     const std::string &name, std::time_t created_at)
    : user_id_(user_id), email_(email), role_(role), name_(name), created_at_(created_at) 
{
    bio_ = "";
    profile_picture_ = "";
    social_links_ = std::vector<std::string>();
}

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

std::string indiepub::User::bio() const
{
    return bio_;
}

std::string indiepub::User::profile_picture() const
{
    return profile_picture_;
}

std::vector<std::string> indiepub::User::social_links() const
{
    return social_links_;
}

void indiepub::User::user_id(const std::string &user_id)
{
    user_id_ = user_id;
}

void indiepub::User::email(const std::string &email)
{
    email_ = email;
}

void indiepub::User::role(const std::string &role)
{
    role_ = role;
}

void indiepub::User::name(const std::string &name)
{
    name_ = name;
}

void indiepub::User::created_at(const std::time_t &created_at)
{
    created_at_ = created_at;
}

void indiepub::User::bio(const std::string &bio)
{
    bio_ = bio;
}

void indiepub::User::profile_picture(const std::string &profile_picture)
{
    profile_picture_ = profile_picture;
}

void indiepub::User::social_links(const std::vector<std::string> &social_links)
{
    social_links_ = social_links;
}

std::string indiepub::User::to_json() const
{
    try 
    {
        std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
        json->put("user_id", user_id_);
        json->put("email", email_);
        json->put("role", role_);
        json->put("name", name_);
        json->put("created_at", indiepub::timestamp_to_string(created_at_));
        json->put("bio", bio_);
        json->put("profile_picture", profile_picture_);
        if (!social_links_.empty())
        {
            std::shared_ptr<JSONArray> socialLinksArray = std::make_shared<JSONArray>();
            for (const auto &link : social_links_)
            {
                socialLinksArray->add(JSON(link));
            }
            json->put("social_links", socialLinksArray);
        }
        return json->str();
    }
    catch(std::exception &e) {(
        throw std::runtime_error(e.what()));
    }
}

indiepub::User indiepub::User::from_json(const std::string &json)
{
    try
    {
        User user;
        std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
        user.user_id_ = jsonObject->get("id").str();
        user.email_ = jsonObject->get("email").str();
        user.role_ = jsonObject->get("role").str();
        user.name_ = jsonObject->get("name").str();
        user.created_at_ = indiepub::string_to_timestamp(jsonObject->get("created_at").str());
        user.bio_ = jsonObject->get("bio").str();
        user.profile_picture_ = jsonObject->get("profile_picture").str();
        if (jsonObject->contains("social_links"))
        {
            std::string socialLinks = jsonObject->get("social_links").c_str();
            std::regex rx(",");
            std::vector<std::string> socialLinksArray = String::tokenize(socialLinks, rx);
            
            for (const auto &link : socialLinksArray)
            {
                user.social_links_.push_back(link);
            }
        }
        
        return user;
    }
    catch (std::runtime_error &ex)
    {
        throw std::runtime_error(ex.what());
    }
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
        
        // Extract created_at (TIMESTAMP)
        cass_int64_t created_at;
        const CassValue *created_at_value = cass_row_get_column_by_name(row, "created_at");
        if (cass_value_get_int64(created_at_value, &created_at) != CASS_OK)
        {
            throw std::runtime_error("Failed to get created_at");
        }

        // Extract email (TEXT)
        const char *email;
        size_t email_length = 0;
        const CassValue *email_value = cass_row_get_column_by_name(row, "email");
        if (cass_value_get_string(email_value, &email, &email_length) != CASS_OK)
        {
            LOG_WARN << "Failed to get email";
            email = "";
        }

        // Extract role (TEXT)
        const char *role;
        size_t role_length = 0;
        const CassValue *role_value = cass_row_get_column_by_name(row, "role");
        if (cass_value_get_string(role_value, &role, &role_length) != CASS_OK)
        {
            LOG_WARN << "Failed to get role";
            role = "";
        }

        // Extract name (TEXT)
        const char *name;
        size_t name_length = 0;
        const CassValue *name_value = cass_row_get_column_by_name(row, "name");
        if (cass_value_get_string(name_value, &name, &name_length) != CASS_OK)
        {
            LOG_WARN << "Failed to get name";
            name = "";
        }


        // Extract bio (TEXT)
        const char *bio;
        size_t bio_length = 0;
        const CassValue *bio_value = cass_row_get_column_by_name(row, "bio");
        if (cass_value_get_string(bio_value, &bio, &bio_length) != CASS_OK)
        {
            LOG_WARN << "Failed to get bio";
            bio = "";
        }

        // Extract profile_picture (TEXT)
        const char *profile_picture;
        size_t profile_picture_length = 0;
        const CassValue *profile_picture_value = cass_row_get_column_by_name(row, "profile_picture");
        if (cass_value_get_string(profile_picture_value, &profile_picture, &profile_picture_length) != CASS_OK)
        {
            LOG_WARN << "Failed to get profile_picture";
            profile_picture = "";
        }

        // Extract social_links (LIST<TEXT>)
        std::vector<std::string> social_links;
        const CassValue *social_links_value = cass_row_get_column_by_name(row, "social_links");
        size_t social_links_count = cass_value_item_count(social_links_value);
        // if (social_links_value != nullptr)  
        if (social_links_count > 0)
        {
            CassIterator *iterator = cass_iterator_from_collection(social_links_value);
            
            while (cass_iterator_next(iterator))
            {
                const CassValue *link_value = cass_iterator_get_value(iterator);
                const char *link;
                size_t link_length;
                if (cass_value_get_string(link_value, &link, &link_length) == CASS_OK)
                {
                    social_links.emplace_back(link, link_length);
                }
            }
            cass_iterator_free(iterator);
        }   

        // Construct and return the User object
        User user(
            std::string(user_id_str),
            std::string(email, email_length),
            std::string(role, role_length),
            std::string(name, name_length),
            static_cast<std::time_t>(created_at)
        );
        user.bio_ = std::string(bio, bio_length);
        user.profile_picture_ = std::string(profile_picture, profile_picture_length);
        user.social_links_ = social_links;

        return user;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        return User();
    }
}
