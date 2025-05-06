#ifndef CASSANDRACONNECTION_HPP
#define CASSANDRACONNECTION_HPP

#include <cassandra.h>
#include <string>

class CassandraConnection {
private:
    CassCluster* cluster;
    CassFuture* connect_future;
    
protected:
    CassSession* session;
    std::string keyspace_;
    

public:
    CassandraConnection(const std::string& contact_points, const std::string& username, const std::string& password);
    CassandraConnection(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

    ~CassandraConnection();

    bool isConnected();

    void executeQuery(const std::string& query);
};

#endif // CASSANDRACONNECTION_HPP