#include <backend/controllers/CredentialsController.hpp>
#include <util/logging/Log.hpp>

indiepub::CredentialsController::CredentialsController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace)
{   
}

bool indiepub::CredentialsController::insertCredentials(const indiepub::Credentials &creds)
{
    bool isExecuted = false;
    if (creds.user_id() == "")
    {
        LOG_ERROR << "User ID cannot be empty";
        return isExecuted;
    }
    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::Credentials::COLUMN_FAMILY + 
    "(user_id, auth_token, pw_hash) VALUES (?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 3);
    CassUuid uuid;
    if (cass_uuid_from_string(creds.user_id().c_str(), &uuid) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + creds.user_id();
        return isExecuted;
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, creds.auth_token().c_str());
    cass_statement_bind_string(statement, 2, creds.pw_hash().c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    else 
    {
        isExecuted = true;
        LOG_DEBUG << "Query executed successfully.";
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isExecuted;
}

indiepub::Credentials indiepub::CredentialsController::getCredentialsByUserId(const std::string &user_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Credentials::COLUMN_FAMILY
        + " WHERE " + indiepub::Credentials::PK_CREDENTIAL_ID + "=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + user_id;
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }

    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::Credentials creds;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            creds = indiepub::Credentials::from_row(row);
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

    return creds;
}

indiepub::Credentials indiepub::CredentialsController::getCredentialsByAuthToken(const std::string &auth_token)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Credentials::COLUMN_FAMILY
        + " WHERE " + indiepub::Credentials::IDX_CREDENTIAL_AUTH_TOKEN + "=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    cass_statement_bind_string(statement, 0, auth_token.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::Credentials creds;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            creds = indiepub::Credentials::from_row(row);
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

    return creds;
}

indiepub::Credentials indiepub::CredentialsController::getCredentialsByPwHash(const std::string &pw_hash)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Credentials::COLUMN_FAMILY
        + " WHERE " + indiepub::Credentials::IDX_CREDENTIAL_PW_HASH + "=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    cass_statement_bind_string(statement, 0, pw_hash.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::Credentials creds;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            creds = indiepub::Credentials::from_row(row);
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

    return creds;
}
