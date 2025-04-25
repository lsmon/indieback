#include <backend/CassandraConnection.hpp>
#include <string>
#include <iostream>
#include <stdexcept>

int main() {
    std::string contact_points = "172.18.0.2";
    std::string username = "cassandra";
    std::string password = "cassandra";
    CassandraConnection cassandra(contact_points, username, password);
    if (!cassandra.isConnected()) {
        std::cerr << "Failed to connect to Cassandra." << std::endl;
        return 1;
    }
    std::cout << "Connected to Cassandra!" << std::endl;
    cassandra.executeQuery("SELECT release_version FROM system.local");

}