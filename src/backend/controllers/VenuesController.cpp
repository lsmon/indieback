#include <backend/controllers/UsersController.hpp>
#include <backend/models/User.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::UsersController::UsersController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::UsersController::insertUser(const indiepub::User &user) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".users (user_id, email, role, name, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid uuid;
    if (cass_uuid_from_string(user.user_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user.user_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, user.email().c_str());
    cass_statement_bind_string(statement, 2, user.role().c_str());
    cass_statement_bind_string(statement, 3, user.name().c_str());
    cass_statement_bind_int64(statement, 4, user.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

std::vector<indiepub::User> indiepub::UsersController::getAllUsers() {
    std::string query = "SELECT * FROM " + keyspace_ + ".users";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::User> users;
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator)) {
                const CassRow *row = cass_iterator_get_row(iterator);
                users.push_back(indiepub::User::from_row(row));
            }
            cass_iterator_free(iterator);
        }
        cass_result_free(result);
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return users;
}

indiepub::User indiepub::UsersController::getUserById(const std::string &user_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + ".users WHERE user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_result_first_row(result);
            indiepub::User user = indiepub::User::from_row(row);
            cass_result_free(result);
            cass_statement_free(statement);
            cass_future_free(query_future);
            return user;
        }
        cass_result_free(result);
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    throw std::runtime_error("User not found");
}

indiepub::User indiepub::UsersController::getUserByEmail(const std::string &email) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + ".users WHERE email = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    cass_statement_bind_string(statement, 0, email.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_result_first_row(result);
            indiepub::User user = indiepub::User::from_row(row);
            cass_result_free(result);
            cass_statement_free(statement);
            cass_future_free(query_future);
            return user;
        }
        cass_result_free(result);
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    throw std::runtime_error("User not found");
}
