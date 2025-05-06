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
#include <backend/models/DailyTicketSales.hpp>
#include <backend/controllers/BandsController.hpp>
#include <backend/controllers/BandMembersController.hpp>
#include <backend/controllers/VenuesController.hpp>
#include <backend/controllers/PostsByDateController.hpp>
#include <backend/controllers/TicketsByUserController.hpp>
#include <backend/controllers/TicketsByEventController.hpp>
#include <backend/controllers/EventController.hpp>
#include <backend/controllers/UsersController.hpp>
#include <backend/controllers/DailyTicketSalesController.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
#include <cassert>
#include <memory>
#include <JSON.hpp>
#include <util/UUID.hpp>
#include <cassert>

void testCassandraConnection()
{
    std::string contact_points = "172.18.0.2";
    std::string username = "cassandra";
    std::string password = "cassandra";
    CassandraConnection cassandra(contact_points, username, password);
    if (!cassandra.isConnected())
    {
        std::cerr << "Failed to connect to Cassandra." << std::endl;
        assert(false);
        throw std::runtime_error("Failed to connect to Cassandra");
    }
    else
    {
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
                user_id UUID,
                email TEXT,
                role TEXT,  -- 'fan', 'band', 'venue'
                name TEXT,
                created_at TIMESTAMP,
                PRIMARY KEY (user_id, created_at)
            ) WITH CLUSTERING ORDER BY (created_at DESC);)");
        std::string createIndexEmail = "CREATE INDEX IF NOT EXISTS idx_users_email ON indie_pub.users (email);";
        std::string createIndexRole = "CREATE INDEX IF NOT EXISTS idx_users_role ON indie_pub.users (role);";
        std::string createIndexName = "CREATE INDEX IF NOT EXISTS idx_users_name ON indie_pub.users (name);";

        cassandra.executeQuery(createUsersTable);
        cassandra.executeQuery(createIndexEmail);
        cassandra.executeQuery(createIndexRole);
        cassandra.executeQuery(createIndexName);

        std::string createVenuesTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.venues (
                venue_id UUID,
                owner_id UUID,  -- Links to user_id of the venue owner
                name TEXT,
                location TEXT,
                capacity INT,
                created_at TIMESTAMP,
                PRIMARY KEY (venue_id, created_at)
            ) WITH CLUSTERING ORDER BY (created_at DESC);)");
        std::string createIdxVenuesOwnerId = "CREATE INDEX IF NOT EXISTS idx_venues_owner_id ON indie_pub.venues (owner_id);";
        std::string createIdxVenuesName = "CREATE INDEX IF NOT EXISTS idx_venues_name ON indie_pub.venues (name);";
        std::string createIdxVenuesLocation = "CREATE INDEX IF NOT EXISTS idx_venues_location ON indie_pub.venues (location);";
        std::string createIdxVenuesCapacity = "CREATE INDEX IF NOT EXISTS idx_venues_capacity ON indie_pub.venues (capacity);";

        cassandra.executeQuery(createVenuesTable);
        cassandra.executeQuery(createIdxVenuesOwnerId);
        cassandra.executeQuery(createIdxVenuesName);
        cassandra.executeQuery(createIdxVenuesLocation);
        cassandra.executeQuery(createIdxVenuesCapacity);

        std::string createBandsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.bands (
                band_id UUID,
                name TEXT,
                genre TEXT,
                description TEXT,
                created_at TIMESTAMP,
                PRIMARY KEY (band_id, created_at)
            ) WITH CLUSTERING ORDER BY (created_at DESC);)");
        std::string createIdxBandsName = "CREATE INDEX IF NOT EXISTS idx_bands_name ON indie_pub.bands (name);";
        std::string createIdxBandsGenre = "CREATE INDEX IF NOT EXISTS idx_bands_genre ON indie_pub.bands (genre);";

        cassandra.executeQuery(createBandsTable);
        cassandra.executeQuery(createIdxBandsName);
        cassandra.executeQuery(createIdxBandsGenre);

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
            ) WITH CLUSTERING ORDER BY (date DESC);)");
        std::string createIdxEventsBandId = "CREATE INDEX IF NOT EXISTS idx_events_by_band_id ON indie_pub.events_by_venue (band_id);";
        std::string createIdxEventsCreatorId = "CREATE INDEX IF NOT EXISTS idx_events_by_creator_id ON indie_pub.events_by_venue (creator_id);";
        std::string createIdxEventsName = "CREATE INDEX IF NOT EXISTS idx_events_by_name ON indie_pub.events_by_venue (name);";
        std::string createIdxEventsPrice = "CREATE INDEX IF NOT EXISTS idx_events_by_price ON indie_pub.events_by_venue (price);";
        std::string createIdxEventsCapacity = "CREATE INDEX IF NOT EXISTS idx_events_by_capacity ON indie_pub.events_by_venue (capacity);";
        std::string createIdxEventsSold = "CREATE INDEX IF NOT EXISTS idx_events_by_sold ON indie_pub.events_by_venue (sold);";
        cassandra.executeQuery(createEventsTable);
        cassandra.executeQuery(createIdxEventsBandId);
        cassandra.executeQuery(createIdxEventsCreatorId);
        cassandra.executeQuery(createIdxEventsName);
        cassandra.executeQuery(createIdxEventsPrice);
        cassandra.executeQuery(createIdxEventsCapacity);
        cassandra.executeQuery(createIdxEventsSold);

        std::string createTicketsTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.tickets_by_user (
                user_id UUID,
                ticket_id UUID,
                event_id UUID,
                purchase_date TIMESTAMP,
                PRIMARY KEY (user_id, ticket_id, parchese_date)
            ) WITH CLUSTERING BY (purchase_date DESC);)");
        std::string createIdxTicketsEventId = "CREATE INDEX IF NOT EXISTS idx_tickets_by_event_id ON indie_pub.tickets_by_user (event_id);";
        cassandra.executeQuery(createTicketsTable);
        cassandra.executeQuery(createIdxTicketsEventId);

        std::string createTicketsByEventTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.tickets_by_event (
                event_id UUID,
                ticket_id UUID,
                user_id UUID,
                purchase_date TIMESTAMP,
                PRIMARY KEY (event_id, ticket_id, purchase_date)
            ) WITH CLUSTERING BY (purchase_date DESC);)");
        std::string createIdxTicketsByEventUserId = "CREATE INDEX IF NOT EXISTS idx_tickets_by_event_id ON indie_pub.tickets_by_event (user_id);";
        cassandra.executeQuery(createTicketsByEventTable);
        cassandra.executeQuery(createIdxTicketsByEventUserId);

        std::string createPostsByUserTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.posts_by_date (
                post_id UUID,
                date DATE,
                user_id UUID,
                content TEXT,
                created_at TIMESTAMP,
                PRIMARY KEY (post_id, date, created_at)
            ) WITH CLUSTERING ORDER BY (created_at DESC);)");
        std::string createIdxPostsByUserId = "CREATE INDEX IF NOT EXISTS idx_posts_by_user_id ON indie_pub.posts_by_date (user_id);";
        cassandra.executeQuery(createPostsByUserTable);
        cassandra.executeQuery(createIdxPostsByUserId);

        std::string createDailyTicketSalesTable(R"(
            CREATE TABLE IF NOT EXISTS indie_pub.daily_ticket_sales (
                event_id UUID,
                sale_date DATE,
                tickets_sold COUNTER,
                PRIMARY KEY (event_id, sale_date)
            ) WITH CLUSTERING BY (sale_date DESC);)");
        cassandra.executeQuery(createDailyTicketSalesTable);
        assert(true);
    }
    catch (const std::exception &e)
    {
        assert(false);
        std::cerr << e.what() << '\n';
    }
}

std::unique_ptr<indiepub::User> user;
std::unique_ptr<indiepub::Venue> venue;
std::unique_ptr<indiepub::Band> band;
std::unique_ptr<indiepub::EventByVenue> event;
std::unique_ptr<indiepub::TicketByEvent> ticket;
std::unique_ptr<indiepub::PostsByDate> post;
std::unique_ptr<indiepub::BandMember> bandMember;
std::unique_ptr<indiepub::DailyTicketSales> dailyTicketSales;

void testUserModel()
{
    // Create a User object
    user = std::make_unique<indiepub::User>(UUID::random(), "abc@def.com", "fan", "John Doe", std::time(nullptr));
    std::cout << "User ID: " << user->user_id() << std::endl;
    std::cout << "Email: " << user->email() << std::endl;
    std::cout << "Role: " << user->role() << std::endl;
    std::cout << "Name: " << user->name() << std::endl;
    std::cout << "Created At: " << user->created_at() << std::endl;
    std::cout << "User JSON: " << user->to_json() << std::endl;
}

void testVenueModel()
{
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

void testBandModel()
{
    // Create a Band object
    band = std::make_unique<indiepub::Band>(UUID::random(), "The Rockers", "Rock", "A popular rock band", std::time(nullptr));
    std::cout << "Band ID: " << band->band_id() << std::endl;
    std::cout << "Name: " << band->name() << std::endl;
    std::cout << "Genre: " << band->genre() << std::endl;
    std::cout << "Description: " << band->description() << std::endl;
    std::cout << "Created At: " << band->created_at() << std::endl;
    std::cout << "Band JSON: " << band->to_json() << std::endl;
}

void testEventModel()
{
    // Create an Event object
    event = std::make_unique<indiepub::EventByVenue>(UUID::random(), UUID::random(), UUID::random(), UUID::random(), "Concert", std::time(nullptr), 50.0, 100, 10);
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

void testTicketModel()
{
    // Create a Ticket object
    ticket = std::make_unique<indiepub::TicketByEvent>(UUID::random(), UUID::random(), UUID::random(), std::time(nullptr));
    std::cout << "Ticket ID: " << ticket->ticket_id() << std::endl;
    std::cout << "User ID: " << ticket->user_id() << std::endl;
    std::cout << "Event ID: " << ticket->event_id() << std::endl;
    std::cout << "Purchase Date: " << ticket->purchase_date() << std::endl;
    std::cout << "Ticket JSON: " << ticket->to_json() << std::endl;
}

void testPostModel()
{
    // Create a Post object
    post = std::make_unique<indiepub::PostsByDate>(UUID::random(), UUID::random(), "This is a test post", std::time(nullptr), "2023-10-01");
    std::cout << "Post ID: " << post->post_id() << std::endl;
    std::cout << "User ID: " << post->user_id() << std::endl;
    std::cout << "Content: " << post->content() << std::endl;
    std::cout << "Created At: " << post->created_at() << std::endl;
    std::cout << "Date: " << post->date() << std::endl;
    std::cout << "Post JSON: " << post->to_json() << std::endl;
}

void testBandMemberModel()
{
    // Create a BandMember object
    bandMember = std::make_unique<indiepub::BandMember>(UUID::random(), UUID::random());
    std::cout << "Band ID: " << bandMember->band_id() << std::endl;
    std::cout << "User ID: " << bandMember->user_id() << std::endl;
    std::cout << "Band Member JSON: " << bandMember->to_json() << std::endl;
}

void testDailyTicketSalesModel()
{
    // Create a DailyTicketSales object
    dailyTicketSales = std::make_unique<indiepub::DailyTicketSales>(UUID::random(), indiepub::timestamp_to_string(std::time(nullptr)), 100);
    std::cout << "Event ID: " << dailyTicketSales->event_id() << std::endl;
    std::cout << "Date: " << dailyTicketSales->sale_date() << std::endl;
    std::cout << "Total Tickets: " << dailyTicketSales->tickets_sold() << std::endl;
    std::cout << "Daily Ticket Sales JSON: " << dailyTicketSales->to_json() << std::endl;
}

void testModels()
{
    testUserModel();
    testVenueModel();
    testBandModel();
    testEventModel();
    testTicketModel();
    testPostModel();
    testBandMemberModel();
    testDailyTicketSalesModel();
}

std::string contact_points = "172.18.0.2";
std::string username = "cassandra";
std::string password = "cassandra";
std::string keyspace = "indie_pub";

void testUsersControllers()
{
    try
    {
        indiepub::UsersController usersController(contact_points, username, password, keyspace);
        usersController.insertUser(*user);
        std::cout << "User inserted successfully!" << std::endl;

        for (auto u : usersController.getAllUsers())
        {
            std::cout << u.to_json() << std::endl;
        }

        
        std::cout << "User retrieved by email successfully!" << std::endl;
        std::cout << usersController.getUserByEmail(user->email()).to_json() << std::endl;
        std::cout << "User retrieved by ID successfully!" << std::endl;
        std::cout << usersController.getUserById(user->user_id()).to_json() << std::endl;
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}

void testVenuesControllers()
{
    try
    {
        indiepub::VenuesController venuesController(contact_points, username, password, keyspace);
        venuesController.insertVenue(*venue);
        std::cout << "Venue inserted successfully!" << std::endl;
        for (auto v : venuesController.getAllVenues())
        {
            std::cout << v.to_json() << std::endl;
        }
        std::cout << "Venue retrieved by ID successfully!" << std::endl;
        std::cout << venuesController.getVenueById(venue->venue_id()).to_json() << std::endl;
        std::cout << "Venue retrieved by name and location successfully!" << std::endl;
        std::cout << venuesController.getVenueBy(venue->name(), venue->location()).to_json() << std::endl;
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}

void testBandControllers()
{
    try
    {
        indiepub::BandsController bandsController(contact_points, username, password, keyspace);
        bandsController.insertBand(*band);
        std::cout << "Band inserted successfully!" << std::endl;
        for (auto b : bandsController.getAllBands())
        {
            std::cout << b.to_json() << std::endl;
        }
        std::cout << "Band retrieved by ID successfully!" << std::endl;
        std::cout << bandsController.getBandById(band->band_id()).to_json() << std::endl;
        std::cout << "Band retrieved by name and genre successfully!" << std::endl;
        std::cout << bandsController.getBandBy(band->name(), band->genre()).to_json() << std::endl;
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}
void testEventControllers()
{
    try
    {
        indiepub::EventController eventController(contact_points, username, password, keyspace);
        eventController.insertEvent(*event);
        std::cout << "Event inserted successfully!" << std::endl;
        for (auto e : eventController.getAllEvents())
        {
            std::cout << e.to_json() << std::endl;
        }
        std::cout << "Event retrieved by ID successfully!" << std::endl;
        std::cout << eventController.getEventById(event->event_id()).to_json() << std::endl;
        std::cout << "Event retrieved by name and location successfully!" << std::endl;
        std::cout << eventController.getEventBy(event->name(), venue->location()).to_json() << std::endl;
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}
void testTicketControllers()
{
    try
    {
        indiepub::TicketsByUserController ticketsController(contact_points, username, password, keyspace);
        indiepub::TicketByUser ticketByUser(ticket->user_id(), ticket->ticket_id(), ticket->event_id(), ticket->purchase_date());
        ticketsController.insertTicket(ticketByUser);
        std::cout << "Ticket inserted successfully!" << std::endl;
        for (auto t : ticketsController.getAllTickets())
        {
            std::cout << t.to_json() << std::endl;
        }
        std::cout << "Ticket retrieved by ID successfully!" << std::endl;
        std::cout << ticketsController.getTicketById(ticket->ticket_id()).to_json() << std::endl;
        std::cout << "Tickets retrieved by user ID successfully!" << std::endl;
        for (auto t : ticketsController.getTicketsByUserId(ticket->user_id()))
        {
            std::cout << t.to_json() << std::endl;
        }
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}

void testPostControllers()
{
    try
    {
        indiepub::PostsByDateController postsController(contact_points, username, password, keyspace);
        postsController.insertPost(*post);
        std::cout << "Post inserted successfully!" << std::endl;
        for (auto p : postsController.getAllPosts())
        {
            std::cout << p.to_json() << std::endl;
        }
        std::cout << "Post retrieved by ID successfully!" << std::endl;
        std::cout << postsController.getPostById(post->post_id()).to_json() << std::endl;
        std::cout << "Posts retrieved by user ID successfully!" << std::endl;
        for (auto p : postsController.getPostsByUserId(post->user_id()))
        {
            std::cout << p.to_json() << std::endl;
        }
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}

void testBandMemberControllers()
{
    try
    {
        indiepub::BandMembersController bandsController(contact_points, username, password, keyspace);
        bandsController.insertBandMember(*bandMember);
        std::cout << "Band member inserted successfully!" << std::endl;
        for (auto b : bandsController.getAllBandMembers())
        {
            std::cout << b.to_json() << std::endl;
        }
        std::cout << "Band member retrieved by ID successfully!" << std::endl;
        std::cout << bandsController.getBandMemberById(bandMember->band_id(), bandMember->user_id()).to_json() << std::endl;
        std::cout << "Band members retrieved by band ID successfully!" << std::endl;
        for (auto b : bandsController.getBandMembersByBandId(bandMember->band_id()))
        {
            std::cout << b.to_json() << std::endl;
        }
        std::cout << "Band members retrieved by user ID successfully!" << std::endl;
        for (auto b : bandsController.getBandMembersByBandId(bandMember->user_id()))
        {
            std::cout << b.to_json() << std::endl;
        }
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}

void testDailyTicketSalesControllers()
{
    try
    {
        indiepub::DailyTicketSalesController dailyTicketSalesController(contact_points, username, password, keyspace);
        dailyTicketSalesController.insertDailyTicketSales(*dailyTicketSales);
        std::cout << "Daily ticket sales inserted successfully!" << std::endl;
        for (auto s : dailyTicketSalesController.getAllDailyTicketSales())
        {
            std::cout << s.to_json() << std::endl;
        }
        std::cout << "Daily ticket sales retrieved by ID successfully!" << std::endl;
        std::cout << dailyTicketSalesController.getDailyTicketSalesByEventId(dailyTicketSales->event_id()).to_json() << std::endl;
        assert(true);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        assert(false);
    }
}

void testControllers()
{
    testUsersControllers();
    testVenuesControllers();
    testBandControllers();
    testBandMemberControllers();
    testEventControllers();
    testTicketControllers();
    testPostControllers();
    testDailyTicketSalesControllers();
}

int main(int argc, char *argv[])
{
    std::string testType = "all";
    std::cerr << "Usage: " << argv[0] << " <test-type>" << std::endl;
    if (argc > 1)
    {
        testType = argv[1];
        return 1;
    }
    if (testType == "all")
    {
        testCassandraConnection();
        testModels();
        testControllers();
    }
    else if (testType == "cassandra")
    {
        testCassandraConnection();
    }
    else if (testType == "models")
    {
        testModels();
    }
    else if (testType == "controller")
    {
        testControllers();
    }
    else
    {
        std::cerr << "Unknown test type: " << testType << std::endl;
        return 1;
    }
    return 0;
}