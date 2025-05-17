#include <backend/models/PostsByDate.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::PostsByDate::COLUMN_FAMILY = "posts_by_date";
const std::string indiepub::PostsByDate::IDX_POSTS_USER_ID = "user_id";
const std::string indiepub::PostsByDate::IDX_POSTS_CONTENT = "content";

indiepub::PostsByDate::PostsByDate(const std::string &post_id, const std::string &user_id,
                     const std::string &content, std::time_t created_at)
    : post_id_(post_id), user_id_(user_id), content_(content),
      created_at_(created_at) {}

std::string indiepub::PostsByDate::post_id() const
{
    return post_id_;
}

std::string indiepub::PostsByDate::user_id() const
{
    return user_id_;
}

std::string indiepub::PostsByDate::content() const
{
    return content_;
}

std::time_t indiepub::PostsByDate::created_at() const
{
    return created_at_;
}

std::string indiepub::PostsByDate::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("post_id", post_id_);
    json->put("user_id", user_id_);
    json->put("content", content_);
    json->put("created_at", timestamp_to_string(created_at_));
    return json->str();
}

indiepub::PostsByDate indiepub::PostsByDate::from_json(const std::string &json)
{
    PostsByDate post;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    post.post_id_ = jsonObject->get("post_id").str();
    post.user_id_ = jsonObject->get("user_id").str();
    post.content_ = jsonObject->get("content").str();
    post.created_at_ = string_to_timestamp(jsonObject->get("created_at").str());
    return post;
}

indiepub::PostsByDate indiepub::PostsByDate::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
            throw std::runtime_error("Row is null");
        }
        CassUuid post_id;
        const CassValue *post_id_value = cass_row_get_column_by_name(row, "post_id");
        if (cass_value_get_uuid(post_id_value, &post_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get post_id from row");
        }
        char post_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(post_id, post_id_str);

        CassUuid user_id;
        const CassValue *user_id_value = cass_row_get_column_by_name(row, "user_id");
        if (cass_value_get_uuid(user_id_value, &user_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get user_id from row");
        }
        char user_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(user_id, user_id_str);

        const CassValue *content_value = cass_row_get_column_by_name(row, "content");
        const char *content;
        size_t content_length;
        cass_value_get_string(content_value, &content, &content_length);
        
        const CassValue *created_at_value = cass_row_get_column_by_name(row, "created_at");
        cass_int64_t created_at;
        if (cass_value_get_int64(created_at_value, &created_at) != CASS_OK)
        {
            throw std::runtime_error("Failed to get created_at from row");
        }
        
        return PostsByDate(std::string(post_id_str), std::string(user_id_str),
                           std::string(content, content_length), created_at);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return PostsByDate();
    }
}