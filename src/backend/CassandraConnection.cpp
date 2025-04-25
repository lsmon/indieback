#include <backend/CassandraConnection.hpp>
#include <stdexcept>
#include <iostream>
#include <string>

CassandraConnection::CassandraConnection(const std::string &contact_points,
                                         const std::string &username,
                                         const std::string &password)
    : cluster(nullptr), session(nullptr), connect_future(nullptr)
{
    cluster = cass_cluster_new();
    session = cass_session_new();

    cass_cluster_set_contact_points(cluster, contact_points.c_str());
    cass_cluster_set_credentials(cluster, username.c_str(), password.c_str());

    connect_future = cass_session_connect(session, cluster);
    if (cass_future_error_code(connect_future) != CASS_OK) {
        // Handle connection error
        const char* message;
        size_t message_length;
        cass_future_error_message(connect_future, &message, &message_length);
        throw std::runtime_error(std::string("Unable to connect to Cassandra: ") + std::string(message, message_length));
    }
}

CassandraConnection::~CassandraConnection()
{
    cass_future_free(connect_future);
    cass_session_free(session);
    cass_cluster_free(cluster);
}

bool CassandraConnection::isConnected()
{
    return cass_future_error_code(connect_future) == CASS_OK;
}

void CassandraConnection::executeQuery(const std::string &query)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    CassStatement* statement = cass_statement_new(query.c_str(), 0);
    CassFuture* query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult* result = cass_future_get_result(query_future);
        const CassRow* row = cass_result_first_row(result);
        const char* release_version;
        size_t release_version_length;

        if (row && cass_value_get_string(cass_row_get_column(row, 0), &release_version, &release_version_length) == CASS_OK) {
            std::cout << "Cassandra version: " << std::string(release_version, release_version_length) << std::endl;
        }

        cass_result_free(result);
    } else {
        std::cerr << "Query execution failed." << std::endl;
    }

    cass_statement_free(statement);
    cass_future_free(query_future);
}