#include <backend/controllers/UsersController.hpp>
#include <backend/models/User.hpp>
#include <util/logging/Log.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::UsersController::UsersController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace)
{
}

bool indiepub::UsersController::insertUser(const indiepub::User &user)
{
    bool isValid = false;
    // Check if the user_id is a valid UUID
    if (user.user_id() == "")
    {
        LOG_ERROR <<  "User ID cannot be empty";
        return isValid;
    }
    if (user.email().empty())
    {
        LOG_ERROR <<  "Email cannot be empty";
        return isValid;
    }
    if (user.role().empty())
    {
        LOG_ERROR <<  "Role cannot be empty";
        return isValid;
    }
    if (user.created_at() <= 0)
    {
        LOG_ERROR <<  "Created at timestamp must be positive";
        return isValid;
    }

    if (!isConnected())
    {
        LOG_ERROR <<  "Not connected to Cassandra";
        return isValid;
    }

    User _user = getUserByEmail(user.email()); // Check if user already exists
    if (_user.user_id() == user.user_id())
    {
        LOG_ERROR <<  "User with this ID already exists";
        return isValid;
    }

    _user = getUserBy(user.name(), user.email()); // Check if user already exists
    if (_user.name() == user.name() && _user.email() == user.email())
    {
        LOG_ERROR <<  "User with this name and email already exists";
        return false;
    }

    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::User::COLUMN_FAMILY + " (user_id, email, role, name, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid uuid;
    if (cass_uuid_from_string(user.user_id().c_str(), &uuid) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + user.user_id();
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, user.email().c_str());
    cass_statement_bind_string(statement, 2, user.role().c_str());
    cass_statement_bind_string(statement, 3, user.name().c_str());
    cass_statement_bind_int64(statement, 4, user.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        LOG_ERROR << error_message;
    }
    else
    {
        isValid = true;
        std::cout << "Query executed successfully.";
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

bool indiepub::UsersController::updateUser(const indiepub::User &user)
{
    bool isValid = false;
    // Check if the user_id is a valid UUID
    if (user.user_id() == "")
    {
        LOG_ERROR <<  "User ID cannot be empty";
        return isValid;
    }
    if (user.email().empty())
    {
        LOG_ERROR <<  "Email cannot be empty";
        return isValid;
    }
    if (user.role().empty())
    {
        LOG_ERROR <<  "Role cannot be empty";
        return isValid;
    }
    if (user.created_at() <= 0)
    {
        LOG_ERROR <<  "Created at timestamp must be positive";
        return isValid;
    }
    if (!isConnected())
    {
        LOG_ERROR <<  "Not connected to Cassandra";
        return isValid;
    }

    std::string query = "UPDATE " + keyspace_ + "." + indiepub::User::COLUMN_FAMILY + " SET bio=?, name=?, profile_picture=?, social_links=? WHERE user_id=? AND created_at=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 6);
    cass_statement_bind_string(statement, 0, user.bio().c_str());
    cass_statement_bind_string(statement, 1, user.name().c_str());
    cass_statement_bind_string(statement, 2, user.profile_picture().c_str());
    CassCollection* collection = cass_collection_new(CASS_COLLECTION_TYPE_LIST, user.social_links().size());
    for (const auto& link : user.social_links())
    {
        CassError rc = cass_collection_append_string(collection, link.c_str());
        if (rc != CASS_OK) {
            LOG_ERROR << "Failed to append string to collection: " << cass_error_desc(rc);
        }
    }
    cass_statement_bind_collection(statement, 3, collection);
    CassUuid uuid;
    if (cass_uuid_from_string(user.user_id().c_str(), &uuid) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + user.user_id();
    }
    cass_statement_bind_uuid(statement, 4, uuid);
    cass_statement_bind_int64(statement, 5, user.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        LOG_ERROR << error_message;
    }
    else
    {
        isValid = true;
        std::cout << "Query executed successfully.";
    }
    cass_collection_free(collection);
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::User> indiepub::UsersController::getAllUsers()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".users";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::User> users;

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator))
            {
                const CassRow *row = cass_iterator_get_row(iterator);
                users.push_back(indiepub::User::from_row(row));
            }
            cass_iterator_free(iterator);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return users;
}

indiepub::User indiepub::UsersController::getUserById(const std::string &user_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".users WHERE user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::User user;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            user = indiepub::User::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return user;
}

indiepub::User indiepub::UsersController::getUserByEmail(const std::string &email)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".users WHERE email = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    cass_statement_bind_string(statement, 0, email.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::User user;

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            user = indiepub::User::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return user;
}

indiepub::User indiepub::UsersController::getUserBy(const std::string &name, const std::string &email)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + User::COLUMN_FAMILY + " WHERE name = ? AND email = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, email.c_str());
    
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::User user;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            user = indiepub::User::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return user;
}
