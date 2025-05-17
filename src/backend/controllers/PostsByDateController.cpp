#include <backend/controllers/PostsByDateController.hpp>
#include <backend/models/PostsByDate.hpp>
#include <backend/IndieBackModels.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::PostsByDateController::PostsByDateController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace)
{
    userController = std::make_shared<UsersController>(contact_points, username, password, keyspace);
}

bool indiepub::PostsByDateController::insertPost(const indiepub::PostsByDate &post)
{
    bool isValid = false;

    // Check if the post_id is a valid UUID
    if (post.post_id().empty())
    {
        std::cerr << "Post ID cannot be empty" << std::endl;
        return isValid;
    }
    if (post.user_id().empty())
    {
        std::cerr << "User ID cannot be empty" << std::endl;
        return isValid;
    }
    if (post.content().empty())
    {
        std::cerr << "Content cannot be empty" << std::endl;
        return isValid;
    }

    indiepub::User _user = userController->getUserById(post.user_id()); // Check if user exists
    if (_user.user_id().empty())
    {
        std::cerr << "User with this ID does not exist" << std::endl;
        return isValid;
    }
    indiepub::PostsByDate existingPost = getPostById(post.post_id());
    if (existingPost.post_id() == post.post_id())
    {
        std::cerr << "Post with this ID already exists" << std::endl;
        return isValid;
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY +
                        " (post_id, created_at, content, user_id) VALUES (?, ?, ?, ?)";

    CassStatement *statement = cass_statement_new(query.c_str(), 4);
    CassUuid user_id;
    CassUuid post_id;
    if (cass_uuid_from_string(post.post_id().c_str(), &post_id) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + post.post_id());
    }
    cass_statement_bind_uuid(statement, 0, post_id);
    cass_statement_bind_int64(statement, 1, post.created_at());
    cass_statement_bind_string(statement, 2, post.content().c_str());

    if (cass_uuid_from_string(post.user_id().c_str(), &user_id) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + post.user_id());
    }
    cass_statement_bind_uuid(statement, 3, user_id);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    else
    {
        std::cout << "Query executed successfully." << std::endl;
        isValid = true;
    }

    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::PostsByDate> indiepub::PostsByDateController::getAllPosts()
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::PostsByDate> posts;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);

        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            posts.emplace_back(indiepub::PostsByDate::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return posts;
}

indiepub::PostsByDate indiepub::PostsByDateController::getPostById(const std::string &post_id)
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY + " WHERE post_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    indiepub::PostsByDate post;
    if (cass_uuid_from_string(post_id.c_str(), &uuid) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + post_id << std::endl;
        return post; // Return an empty Post object in case of failure
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            post = indiepub::PostsByDate::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return post;
}

std::vector<indiepub::PostsByDate> indiepub::PostsByDateController::getPostsByUserId(const std::string &user_id)
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + PostsByDate::COLUMN_FAMILY + " WHERE " + PostsByDate::IDX_POSTS_USER_ID + " = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    std::vector<indiepub::PostsByDate> posts;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + user_id << std::endl;
        return posts; // Return an empty vector in case of failure
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            posts.emplace_back(indiepub::PostsByDate::from_row(row));
        }
        cass_iterator_free(iterator);
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return posts;
}