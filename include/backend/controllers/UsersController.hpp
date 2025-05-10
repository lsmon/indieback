#ifndef INDIEPUB_USERS_CONTROLLER_HPP
#define INDIEPUB_USERS_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/User.hpp>
#include <functional>

namespace indiepub {

    class UsersController : public CassandraConnection {
    public:
        UsersController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertUser(const indiepub::User& user);

        std::vector<indiepub::User> getAllUsers();

        indiepub::User getUserById(const std::string& user_id);
        indiepub::User getUserByEmail(const std::string& email);
        indiepub::User getUserBy(const std::string& name, const std::string& email);

    private:
        // Add any private members or methods if needed
    };

} // namespace indiepub

#endif // INDIEPUB_USERS_CONTROLLER_HPP