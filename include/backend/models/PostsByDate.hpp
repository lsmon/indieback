#ifndef INDIEPUB_POSTS_BY_DATE_HPP
#define INDIEPUB_POSTS_BY_DATE_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class PostsByDate
    {
    public:
        PostsByDate() = default;
        PostsByDate(const std::string &post_id, const std::string &user_id, const std::string &content,
             std::time_t created_at, const std::string &date);

        static const std::string COLUMN_FAMILY;
        static const std::string IDX_POSTS_USER_ID;
        static const std::string IDX_POSTS_CONTENT;
        
        // Getters
        std::string post_id() const;
        std::string user_id() const;
        std::string content() const;
        std::time_t created_at() const;
        std::string date() const; // Date as YYYY-MM-DD

        // JSON serialization
        std::string to_json() const;

        static PostsByDate from_json(const std::string &json);
        static PostsByDate from_row(const CassRow *row);

    private:
        std::string post_id_; // UUID
        std::string user_id_; // UUID
        std::string content_;
        std::time_t created_at_;
        std::string date_; // YYYY-MM-DD
    };
}

#endif // INDIEPUB_POSTS_BY_DATE_HPP