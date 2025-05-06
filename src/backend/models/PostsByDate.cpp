#include <backend/models/PostsByDate.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::PostsByDate::COLUMN_FAMILY = "posts_by_date";
const std::string indiepub::PostsByDate::IDX_POSTS_USER_ID = "user_id";
const std::string indiepub::PostsByDate::IDX_POSTS_CONTENT = "content";

indiepub::PostsByDate::PostsByDate(const std::string &post_id, const std::string &user_id,
                     const std::string &content, std::time_t created_at, const std::string &date)
    : post_id_(post_id), user_id_(user_id), content_(content),
      created_at_(created_at), date_(date) {}

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

std::string indiepub::PostsByDate::date() const
{
    return date_;
}

std::string indiepub::PostsByDate::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("post_id", post_id_);
    json->put("user_id", user_id_);
    json->put("content", content_);
    json->put("created_at", timestamp_to_string(created_at_));
    json->put("date", date_);
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
    post.date_ = jsonObject->get("date").str();
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
        const char *post_id;
        size_t post_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &post_id, &post_id_length);

        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &user_id, &user_id_length);

        const char *content;
        size_t content_length;
        cass_value_get_string(cass_row_get_column(row, 2), &content, &content_length);

        cass_int64_t created_at;
        cass_value_get_int64(cass_row_get_column(row, 3), &created_at);

        const char *date;
        size_t date_length;
        cass_value_get_string(cass_row_get_column(row, 4), &date, &date_length);

        return PostsByDate(std::string(post_id, post_id_length),
                    std::string(user_id, user_id_length),
                    std::string(content, content_length),
                    created_at,
                    std::string(date, date_length));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return PostsByDate();
    }
}