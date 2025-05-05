#include <backend/IndieBackController.hpp>
#include <backend/IndieBackModels.hpp>
#include <backend/CassandraConnection.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::IndieBackController::IndieBackController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password), keyspace_(keyspace) {
}

void indiepub::IndieBackController::insertUser(const indiepub::User &user)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".users (user_id, email, role, name, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid uuid;
    if (cass_uuid_from_string(user.user_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user.user_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, user.email().c_str());
    cass_statement_bind_string(statement, 2, user.role().c_str());
    cass_statement_bind_string(statement, 3, user.name().c_str());
    cass_statement_bind_int64(statement, 4, user.created_at());
    
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

void indiepub::IndieBackController::insertVenue(const indiepub::Venue &venue)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".venues (venue_id, owner_id, name, location, capacity, created_at) VALUES (?, ?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 6);
    CassUuid venue_id;
    if (cass_uuid_from_string(venue.venue_id().c_str(), &venue_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + venue.venue_id());
    }
    CassUuid owner_id;
    if (cass_uuid_from_string(venue.owner_id().c_str(), &owner_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + venue.owner_id());
    }
    cass_statement_bind_uuid(statement, 0, venue_id);
    cass_statement_bind_uuid(statement, 1, owner_id);
    cass_statement_bind_string(statement, 2, venue.name().c_str());
    cass_statement_bind_string(statement, 3, venue.location().c_str());
    cass_statement_bind_int32(statement, 4, venue.capacity());
    cass_statement_bind_int64(statement, 5, venue.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

void indiepub::IndieBackController::insertBand(const indiepub::Band &band)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".bands (band_id, name, genre, description, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid uuid;
    if (cass_uuid_from_string(band.band_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band.band_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, band.name().c_str());
    cass_statement_bind_string(statement, 2, band.genre().c_str());
    cass_statement_bind_string(statement, 3, band.description().c_str());
    cass_statement_bind_int64(statement, 4, band.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    
    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

void indiepub::IndieBackController::insertEvent(const indiepub::Event &event)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".events (event_id, venue_id, band_id, creator_id, name, date, price, capacity, sold) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 9);
    CassUuid uuid;
    if (cass_uuid_from_string(event.event_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.event_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    if (cass_uuid_from_string(event.venue_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.venue_id());
    }
    cass_statement_bind_uuid(statement, 1, uuid);
    if (cass_uuid_from_string(event.band_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.band_id());
    }
    cass_statement_bind_uuid(statement, 2, uuid);
    if (cass_uuid_from_string(event.creator_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.creator_id());
    }
    cass_statement_bind_uuid(statement, 3, uuid);
    cass_statement_bind_string(statement, 4, event.name().c_str());
    cass_statement_bind_int64(statement, 5, event.date());
    cass_statement_bind_int64(statement, 6, event.price());
    cass_statement_bind_int64(statement, 7, event.capacity());
    cass_statement_bind_int64(statement, 8, event.sold());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

void indiepub::IndieBackController::insertTicket(const indiepub::Ticket &ticket)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".tickets (ticket_id, user_id, event_id, purchase_date) VALUES (?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 4);
    CassUuid uuid;
    if (cass_uuid_from_string(ticket.ticket_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket.ticket_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    if (cass_uuid_from_string(ticket.user_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket.user_id());
    }
    cass_statement_bind_uuid(statement, 1, uuid);
    if (cass_uuid_from_string(ticket.event_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket.event_id());
    }
    cass_statement_bind_uuid(statement, 2, uuid);
    cass_statement_bind_int64(statement, 3, ticket.purchase_date());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

void indiepub::IndieBackController::insertPost(const indiepub::Post &post)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".posts (post_id, user_id, content, created_at, date) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid uuid;
    if (cass_uuid_from_string(post.post_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + post.post_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    if (cass_uuid_from_string(post.user_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + post.user_id());
    }
    cass_statement_bind_uuid(statement, 1, uuid);
    cass_statement_bind_string(statement, 2, post.content().c_str());
    cass_statement_bind_int64(statement, 3, post.created_at());
    cass_statement_bind_string(statement, 4, post.date().c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    
    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

void indiepub::IndieBackController::insertDailyTicketSales(const indiepub::DailyTicketSales &sales)
{
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".daily_ticket_sales (event_id, sale_date, tickets_sold) VALUES (?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 3);
    CassUuid uuid;
    if (cass_uuid_from_string(sales.event_id().c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + sales.event_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, sales.sale_date().c_str());
    cass_statement_bind_int64(statement, 2, sales.tickets_sold());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

std::vector<indiepub::User> indiepub::IndieBackController::getAllUsers()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".users";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::User> users;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            users.emplace_back(indiepub::User::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return users;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

std::vector<indiepub::Venue> indiepub::IndieBackController::getAllVenues()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".venues";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::Venue> venues;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            venues.emplace_back(indiepub::Venue::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return venues;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

std::vector<indiepub::Band> indiepub::IndieBackController::getAllBands()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".bands";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::Band> bands;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            bands.emplace_back(indiepub::Band::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return bands;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

std::vector<indiepub::Event> indiepub::IndieBackController::getAllEvents()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".events";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::Event> events;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            events.emplace_back(indiepub::Event::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return events;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

std::vector<indiepub::Ticket> indiepub::IndieBackController::getAllTickets()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".tickets";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::Ticket> tickets;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.emplace_back(indiepub::Ticket::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return tickets;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

std::vector<indiepub::Post> indiepub::IndieBackController::getAllPosts()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".posts";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::Post> posts;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            posts.emplace_back(indiepub::Post::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return posts;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

std::vector<indiepub::DailyTicketSales> indiepub::IndieBackController::getAllDailyTicketSales()
{
    std::string query = "SELECT * FROM " + keyspace_ + ".daily_ticket_sales";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::DailyTicketSales> sales;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            sales.emplace_back(indiepub::DailyTicketSales::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return sales;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
}

indiepub::User indiepub::IndieBackController::getUserById(const std::string &user_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".users WHERE user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::User::from_row(row);
        } else {
            throw std::runtime_error("User not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    throw std::runtime_error("User not found");
}

indiepub::User indiepub::IndieBackController::getUserBy(const std::string &email)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".users WHERE email = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    cass_statement_bind_string(statement, 0, email.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::User::from_row(row);
        } else {
            throw std::runtime_error("User not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    // If we reach here, it means the user was not found
    throw std::runtime_error("User not found");
}

indiepub::Venue indiepub::IndieBackController::getVenueById(const std::string &venue_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".venues WHERE venue_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(venue_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + venue_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::Venue::from_row(row);
        } else {
            throw std::runtime_error("Venue not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    // If we reach here, it means the venue was not found
    throw std::runtime_error("Venue not found");
}

indiepub::Venue indiepub::IndieBackController::getVenueBy(const std::string &name, const std::string &location)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".venues WHERE name = ? AND location = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, location.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::Venue::from_row(row);
        } else {
            throw std::runtime_error("Venue not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    // If we reach here, it means the venue was not found
    throw std::runtime_error("Venue not found");
}

indiepub::Band indiepub::IndieBackController::getBandById(const std::string &band_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + ".bands WHERE band_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(band_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::Band::from_row(row);
        } else {
            throw std::runtime_error("Band not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    // If we reach here, it means the band was not found
    throw std::runtime_error("Band not found");
}
