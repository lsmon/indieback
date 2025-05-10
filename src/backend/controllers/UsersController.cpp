#include <backend/controllers/UsersController.hpp>
#include <backend/models/User.hpp>
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "User ID cannot be empty" << std::endl;
        return isValid;
    }
    if (user.email().empty())
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Email cannot be empty" << std::endl;
        return isValid;
    }
    if (user.role().empty())
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Role cannot be empty" << std::endl;
        return isValid;
    }
    if (user.name().empty())
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Name cannot be empty" << std::endl;
        return isValid;
    }
    if (user.created_at() <= 0)
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Created at timestamp must be positive" << std::endl;
        return isValid;
    }

    if (!isConnected())
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Not connected to Cassandra" << std::endl;
        return isValid;
    }

    User _user = getUserByEmail(user.email()); // Check if user already exists
    if (_user.user_id() == user.user_id())
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "User with this ID already exists" << std::endl;
        return isValid;
    }

    _user = getUserBy(user.name(), user.email()); // Check if user already exists
    if (_user.name() == user.name() && _user.email() == user.email())
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "User with this name and email already exists" << std::endl;
        return false;
    }

    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::User::COLUMN_FAMILY + " (user_id, email, role, name, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid uuid;
    if (cass_uuid_from_string(user.user_id().c_str(), &uuid) != CASS_OK)
    {
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Invalid UUID string: " + user.user_id() << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << error_message << std::endl;
    }
    else
    {
        isValid = true;
        std::cout << "Query executed successfully." << std::endl;
    }
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Query execution failed: " << std::string(message, message_length) << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Query execution failed: " << std::string(message, message_length) << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Query execution failed: " << std::string(message, message_length) << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : "  << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return user;
}
