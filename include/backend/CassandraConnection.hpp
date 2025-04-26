#ifndef CASSANDRACONNECTION_HPP
#define CASSANDRACONNECTION_HPP

#include <cassandra.h>
#include <string>

class CassandraConnection {
private:
    CassCluster* cluster;
    CassSession* session;
    CassFuture* connect_future;

public:
    CassandraConnection(const std::string& contact_points, const std::string& username, const std::string& password);

    ~CassandraConnection();

    bool isConnected();

    void executeQuery(const std::string& query);
};

#endif // CASSANDRACONNECTION_HPP