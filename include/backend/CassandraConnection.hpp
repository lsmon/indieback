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
    

public:
    CassandraConnection(const std::string& contact_points, const std::string& username, const std::string& password);

    ~CassandraConnection();

    bool isConnected();

    void executeQuery(const std::string& query);
};

#endif // CASSANDRACONNECTION_HPP