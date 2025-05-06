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
    };
}
#endif // INDIEPUB_USER_HPP