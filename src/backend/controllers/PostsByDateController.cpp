#include <backend/controllers/PostsByDateController.hpp>
#include <backend/models/PostsByDate.hpp>
#include <backend/IndieBackModels.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::PostsByDateController::PostsByDateController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::PostsByDateController::insertPost(const indiepub::PostsByDate &post) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY +
                        " (date, created_at, post_id, user_id, content) VALUES (?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 4);
    CassUuid user_id;
    CassUuid post_id;
    if (cass_uuid_from_string(post.user_id().c_str(), &user_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + post.user_id());
    }
    if (cass_uuid_from_string(post.post_id().c_str(), &post_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + post.post_id());
    }
    cass_statement_bind_int64(statement, 0, indiepub::string_to_timestamp(post.date().c_str()));
    cass_statement_bind_int64(statement, 1, post.created_at());
    cass_statement_bind_uuid(statement, 2, post_id);
    cass_statement_bind_uuid(statement, 3, user_id);
    cass_statement_bind_string(statement, 4, post.content().c_str());

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

std::vector<indiepub::PostsByDate> indiepub::PostsByDateController::getAllPosts() {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::PostsByDate> posts;
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            posts.emplace_back(indiepub::PostsByDate::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return posts;
}

indiepub::PostsByDate indiepub::PostsByDateController::getPostById(const std::string &post_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY + " WHERE post_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(post_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + post_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            indiepub::PostsByDate post = indiepub::PostsByDate::from_row(row);
            cass_result_free(result);
            return post;
        } else {
            throw std::runtime_error("Post not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the post was not found
    throw std::runtime_error("Post not found");
}

std::vector<indiepub::PostsByDate> indiepub::PostsByDateController::getPostsByUserId(const std::string &user_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + PostsByDate::IDX_POSTS_USER_ID + " WHERE user_id = ?";
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
        std::vector<indiepub::PostsByDate> posts;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            posts.emplace_back(indiepub::PostsByDate::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return posts;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means no posts were found
    throw std::runtime_error("No posts found for the given user ID");
}