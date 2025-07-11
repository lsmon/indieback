#ifndef INDIEPUB_USER_HPP
#define INDIEPUB_USER_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub {
    class User {
    public:
        User() = default;
        User(const std::string& user_id, const std::string& email, const std::string& role,
             const std::string& name, std::time_t created_at);
        
        static const std::string COLUMN_FAMILY;
        static const std::string IDX_USERS_EMAIL;
        static const std::string IDX_USERS_ROLE;
        static const std::string IDX_USERS_NAME;
        // Getters
        std::string user_id() const;
        std::string email() const;
        std::string role() const;
        std::string name() const;
        std::time_t created_at() const;
        std::string bio() const;
        std::string profile_picture() const;
        std::vector<std::string> social_links() const; // List of social media links

        // Setters
        void user_id(const std::string& user_id);
        void email(const std::string& email);
        void role(const std::string& role);
        void name(const std::string& name);
        void created_at(const std::time_t& created_at);
        void bio(const std::string& bio);
        void profile_picture(const std::string& profile_picture);
        void social_links(const std::vector<std::string>& social_links);

        // JSON serialization
        std::string to_json() const;

        static User from_json(const std::string& json);

        static User from_row(const CassRow *row);

    private:
        std::string user_id_;  // UUID
        std::string email_;
        std::string role_;    // 'fan', 'band', 'venue'
        std::string name_;
        std::time_t created_at_;
        std::string bio_;
        std::string profile_picture_;
        std::vector<std::string> social_links_; // List of social media links

    };
}
#endif // INDIEPUB_USER_HPP