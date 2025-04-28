#include <backend/IndieBackController.hpp>
#include <backend/IndieBackModels.hpp>
#include <backend/CassandraConnection.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::IndieBackController::IndieBackController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password), keyspace_(keyspace) {
}

void indiepub::IndieBackController::insertUser(const indiepub::User &user)
{
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
        throw std::runtime_error("Query execution failed");
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}