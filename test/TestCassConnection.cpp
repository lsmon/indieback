#include <backend/CassandraConnection.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cassert>

int main() {
    std::string contact_points = "172.18.0.2";
    std::string username = "cassandra";
    std::string password = "cassandra";
    CassandraConnection cassandra(contact_points, username, password);
    if (!cassandra.isConnected()) {
        std::cerr << "Failed to connect to Cassandra." << std::endl;
        assert(false);
        return 1;
    } else {
        std::cout << "Connected to Cassandra!" << std::endl;
        assert(true);
    }

    try
    {    
        std::string createKeyspaceQuery = R"(
            CREATE KEYSPACE IF NOT EXISTS indie_pub with replication = {
                'class': 'SimpleStrategy', 'replication_factor': 1};
            )";

        cassandra.executeQuery(createKeyspaceQuery);

        std::string createUsersTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.users (
                user_id UUID PRIMARY KEY,
                email TEXT,
                role TEXT,  -- 'fan', 'band', 'venue'
                name TEXT,
                created_at TIMESTAMP
            );)");
        cassandra.executeQuery(createUsersTable);

        std::string createVenuesTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.venues (
                venue_id UUID PRIMARY KEY,
                owner_id UUID,  -- Links to user_id of the venue owner
                name TEXT,
                location TEXT,
                capacity INT,
                created_at TIMESTAMP
            );)");
        cassandra.executeQuery(createVenuesTable);

        std::string createBandsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.bands (
                band_id UUID PRIMARY KEY,
                name TEXT,
                genre TEXT,
                description TEXT,
                created_at TIMESTAMP
            );)");

        std::string createBandMembersTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.band_members (
                band_id UUID,
                user_id UUID,
                PRIMARY KEY (band_id, user_id)
            );)");
        cassandra.executeQuery(createBandMembersTable);

        std::string createEventsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.events_by_venue (
                venue_id UUID,
                event_id UUID,
                band_id UUID,
                creator_id UUID,  -- User who created the event
                name TEXT,
                date TIMESTAMP,
                price DECIMAL,
                capacity INT,
                sold INT,
                PRIMARY KEY (venue_id, date, event_id)
            );)");
        cassandra.executeQuery(createEventsTable);

        std::string createTicketsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.tickets_by_user (
                user_id UUID,
                ticket_id UUID,
                event_id UUID,
                purchase_date TIMESTAMP,
                PRIMARY KEY (user_id, ticket_id)
            );)");
        cassandra.executeQuery(createTicketsTable);

        std::string createPostsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.tickets_by_event (
                event_id UUID,
                ticket_id UUID,
                user_id UUID,
                purchase_date TIMESTAMP,
                PRIMARY KEY (event_id, ticket_id)
            );)");
        cassandra.executeQuery(createPostsTable);

        std::string createPostsByUserTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.posts_by_date (
                date DATE,
                post_id UUID,
                user_id UUID,
                content TEXT,
                created_at TIMESTAMP,
                PRIMARY KEY (date, created_at, post_id)
            );)");
        cassandra.executeQuery(createPostsByUserTable);

        std::string createCommentsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.daily_ticket_sales (
                event_id UUID,
                sale_date DATE,
                tickets_sold COUNTER,
                PRIMARY KEY (event_id, sale_date)
            );)");
        cassandra.executeQuery(createCommentsTable);
        assert(true);
    }
    catch(const std::exception& e)
    {
        assert(false);
        std::cerr << e.what() << '\n';
    }
    return 0;
}