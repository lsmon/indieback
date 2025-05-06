#include <backend/controllers/VenuesController.hpp>
#include <backend/models/Venue.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::VenuesController::VenuesController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace)
{
}

void indiepub::VenuesController::insertVenue(const indiepub::Venue &venue)
{
    if (!isConnected())
    {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY + " (venue_id, name, location, capacity) VALUES (?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 4);
    CassUuid uuid;
    if (cass_uuid_from_string(venue.venue_id().c_str(), &uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + venue.venue_id());
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, venue.name().c_str());
    cass_statement_bind_string(statement, 2, venue.location().c_str());
    cass_statement_bind_int32(statement, 3, venue.capacity());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    }
    else
    {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

std::vector<indiepub::Venue> indiepub::VenuesController::getAllVenues()
{
    if (!isConnected())
    {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::Venue> venues;
        CassIterator *iterator = cass_iterator_from_result(result);
        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            venues.push_back(indiepub::Venue::from_row(row));
        }
        cass_iterator_free(iterator);

        cass_result_free(result);
        return venues;
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    throw std::runtime_error("Failed to execute query");
}

indiepub::Venue indiepub::VenuesController::getVenueById(const std::string &venue_id)
{
    if (!isConnected())
    {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY + " WHERE venue_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(venue_id.c_str(), &uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + venue_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            indiepub::Venue venue = indiepub::Venue::from_row(row);
            cass_result_free(result);
            cass_statement_free(statement);
            cass_future_free(query_future);
            return venue;
        }
        else
        {
            throw std::runtime_error("No venue found with the given ID");
        }
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    throw std::runtime_error("Failed to execute query");
}

indiepub::Venue indiepub::VenuesController::getVenueBy(const std::string &name, const std::string &location)
{
    if (!isConnected())
    {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY + " WHERE name = ? AND location = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, location.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            indiepub::Venue venue = indiepub::Venue::from_row(row);
            cass_result_free(result);
            cass_statement_free(statement);
            cass_future_free(query_future);
            return venue;
        }
        else
        {
            throw std::runtime_error("No venue found with the given name and location");
        }
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    throw std::runtime_error("Failed to execute query");
}
