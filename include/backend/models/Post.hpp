#ifndef INDIEPUB_POST_HPP
#define INDIEPUB_POST_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class Post
    {
    public:
        Post() = default;
        Post(const std::string &post_id, const std::string &user_id, const std::string &content,
             std::time_t created_at, const std::string &date);

        // Getters
        std::string post_id() const;
        std::string user_id() const;
        std::string content() const;
        std::time_t created_at() const;
        std::string date() const; // Date as YYYY-MM-DD

        // JSON serialization
        std::string to_json() const;

        static Post from_json(const std::string &json);
        static Post from_row(const CassRow *row);

    private:
        std::string post_id_; // UUID
        std::string user_id_; // UUID
        std::string content_;
        std::time_t created_at_;
        std::string date_; // YYYY-MM-DD
    };
}

#endif // INDIEPUB_POST_HPP