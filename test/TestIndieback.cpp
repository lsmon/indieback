#include <backend/CassandraConnection.hpp>
#include <backend/IndieBackModels.hpp>
#include <backend/models/BandMember.hpp>
#include <backend/models/Band.hpp>
#include <backend/models/EventByVenue.hpp>
#include <backend/models/TicketByUser.hpp>
#include <backend/models/TicketByEvent.hpp>
#include <backend/models/User.hpp>
#include <backend/models/Venue.hpp>
#include <backend/models/PostsByDate.hpp>
#include <backend/controllers/BandsController.hpp>
#include <backend/controllers/VenuesController.hpp>
#include <backend/controllers/PostsByDateController.hpp>
#include <backend/controllers/TicketsByUserController.hpp>
#include <backend/controllers/TicketsByEventController.hpp>
#include <backend/controllers/EventController.hpp>
#include <backend/controllers/UsersController.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <JSON.hpp>
#include <util/UUID.hpp>
#include <cassert>

void testCassandraConnection() {
    std::string contact_points = "172.18.0.2";
    std::string username = "cassandra";
    std::string password = "cassandra";
    CassandraConnection cassandra(contact_points, username, password);
    if (!cassandra.isConnected()) {
        std::cerr << "Failed to connect to Cassandra." << std::endl;
        assert(false);
        throw std::runtime_error("Failed to connect to Cassandra");
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
        cassandra.executeQuery(createBandsTable);

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
}

std::unique_ptr<indiepub::User> user;
std::unique_ptr<indiepub::Venue> venue;
std::unique_ptr<indiepub::Band> band;
std::unique_ptr<indiepub::Event> event;
std::unique_ptr<indiepub::Ticket> ticket;
std::unique_ptr<indiepub::Post> post;
std::unique_ptr<indiepub::BandMember> bandMember;
std::unique_ptr<indiepub::DailyTicketSales> dailyTicketSales;

void testUserCRUD() {
    // Create a User object
    user = std::make_unique<indiepub::User>(UUID::random(), "abc@def.com", "fan", "John Doe", std::time(nullptr));
    std::cout << "User ID: " << user->user_id() << std::endl;
    std::cout << "Email: " << user->email() << std::endl;
    std::cout << "Role: " << user->role() << std::endl;
    std::cout << "Name: " << user->name() << std::endl;
    std::cout << "Created At: " << user->created_at() << std::endl;
    std::cout << "User JSON: " << user->to_json() << std::endl;
}

void testVenueCRUD() {
    // Create a Venue object
    venue = std::make_unique<indiepub::Venue>(UUID::random(), UUID::random(), "The Grand Hall", "123 Main St", 500, std::time(nullptr));
    std::cout << "Venue ID: " << venue->venue_id() << std::endl;
    std::cout << "Owner ID: " << venue->owner_id() << std::endl;
    std::cout << "Name: " << venue->name() << std::endl;
    std::cout << "Location: " << venue->location() << std::endl;
    std::cout << "Capacity: " << venue->capacity() << std::endl;
    std::cout << "Created At: " << venue->created_at() << std::endl;
    std::cout << "Venue JSON: " << venue->to_json() << std::endl;
}

void testBandCRUD() {
    // Create a Band object
    band = std::make_unique<indiepub::Band>(UUID::random(), "The Rockers", "Rock", "A popular rock band", std::time(nullptr));
    std::cout << "Band ID: " << band->band_id() << std::endl;
    std::cout << "Name: " << band->name() << std::endl;
    std::cout << "Genre: " << band->genre() << std::endl;
    std::cout << "Description: " << band->description() << std::endl;
    std::cout << "Created At: " << band->created_at() << std::endl;
    std::cout << "Band JSON: " << band->to_json() << std::endl;
}

void testEventCRUD() {
    // Create an Event object
    event = std::make_unique<indiepub::Event>(UUID::random(), UUID::random(), UUID::random(), UUID::random(), "Concert", std::time(nullptr), 50.0, 100, 10);
    std::cout << "Event ID: " << event->event_id() << std::endl;
    std::cout << "Venue ID: " << event->venue_id() << std::endl;
    std::cout << "Band ID: " << event->band_id() << std::endl;
    std::cout << "Creator ID: " << event->creator_id() << std::endl;
    std::cout << "Name: " << event->name() << std::endl;
    std::cout << "Date: " << event->date() << std::endl;
    std::cout << "Price: " << event->price() << std::endl;
    std::cout << "Capacity: " << event->capacity() << std::endl;
    std::cout << "Sold: " << event->sold() << std::endl;
    std::cout << "Event JSON: " << event->to_json() << std::endl;
}

void testTicketCRUD() {
    // Create a Ticket object
    ticket = std::make_unique<indiepub::Ticket>(UUID::random(), UUID::random(), UUID::random(), std::time(nullptr));
    std::cout << "Ticket ID: " << ticket->ticket_id() << std::endl;
    std::cout << "User ID: " << ticket->user_id() << std::endl;
    std::cout << "Event ID: " << ticket->event_id() << std::endl;
    std::cout << "Purchase Date: " << ticket->purchase_date() << std::endl;
    std::cout << "Ticket JSON: " << ticket->to_json() << std::endl;
}

void testPostCRUD() {
    // Create a Post object
    post = std::make_unique<indiepub::Post>(UUID::random(), UUID::random(), "This is a test post", std::time(nullptr), "2023-10-01");
    std::cout << "Post ID: " << post->post_id() << std::endl;
    std::cout << "User ID: " << post->user_id() << std::endl;
    std::cout << "Content: " << post->content() << std::endl;
    std::cout << "Created At: " << post->created_at() << std::endl;
    std::cout << "Date: " << post->date() << std::endl;
    std::cout << "Post JSON: " << post->to_json() << std::endl;
}

void testBandMemberCRUD() {
    // Create a BandMember object
    bandMember = std::make_unique<indiepub::BandMember>(UUID::random(), UUID::random());
    std::cout << "Band ID: " << bandMember->band_id() << std::endl;
    std::cout << "User ID: " << bandMember->user_id() << std::endl;
    std::cout << "Band Member JSON: " << bandMember->to_json() << std::endl;
}

void testDailyTicketSalesCRUD() {
    // Create a DailyTicketSales object
    dailyTicketSales = std::make_unique<indiepub::DailyTicketSales>(UUID::random(), indiepub::timestamp_to_string(std::time(nullptr)), 100);
    std::cout << "Event ID: " << dailyTicketSales->event_id() << std::endl;
    std::cout << "Date: " << dailyTicketSales->sale_date() << std::endl;
    std::cout << "Total Tickets: " << dailyTicketSales->tickets_sold() << std::endl;
    std::cout << "Daily Ticket Sales JSON: " << dailyTicketSales->to_json() << std::endl;
}

std::string contact_points = "172.18.0.2";
std::string username = "cassandra";
std::string password = "cassandra";
std::string keyspace = "indie_pub";

void testInserts() {
    indiepub::IndieBackController controller(contact_points, username, password, keyspace);

    controller.insertUser(*user);
    controller.insertVenue(*venue);
    controller.insertBand(*band);
    controller.insertEvent(*event);
    controller.insertTicket(*ticket);
    controller.insertPost(*post);
    controller.insertDailyTicketSales(*dailyTicketSales);

    for (auto u : controller.getAllUsers()) {
        std::cout << u.to_json() << std::endl;
    }

    for (auto v : controller.getAllVenues()) {
        std::cout << v.to_json() << std::endl;
    }
    for (auto b : controller.getAllBands()) {
        std::cout << b.to_json() << std::endl;
    }
    for (auto e : controller.getAllEvents()) {
        std::cout << e.to_json() << std::endl;
    }
    for (auto t : controller.getAllTickets()) {
        std::cout << t.to_json() << std::endl;
    }
    for (auto p : controller.getAllPosts()) {
        std::cout << p.to_json() << std::endl;
    }
    for (auto s : controller.getAllDailyTicketSales()) {
        std::cout << s.to_json() << std::endl;
    }
    std::cout << "All data inserted successfully!" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string testType = "all";
    std::cerr << "Usage: " << argv[0] << " <test-type>" << std::endl;
    if (argc > 1) {
        testType = argv[1];
        return 1;
    }
    if (testType == "all") {
        testCassandraConnection();
        testUserCRUD();
        testVenueCRUD();
        testBandCRUD();
        testEventCRUD();
        testTicketCRUD();
        testPostCRUD();
        testBandMemberCRUD();
        testDailyTicketSalesCRUD();
        testInserts();
    } else if (testType == "cassandra") {
        testCassandraConnection();
    } else if (testType == "models") {
        testUserCRUD();
        testVenueCRUD();
        testBandCRUD();
        testEventCRUD();
        testTicketCRUD();
        testPostCRUD();
        testBandMemberCRUD();
        testDailyTicketSalesCRUD();
        testInserts();
    } else if (testType == "controller") {
        testInserts();
    } else {
        std::cerr << "Unknown test type: " << testType << std::endl;
        return 1;
    }
    return 0;
}