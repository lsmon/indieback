#include <backend/controllers/VenuesController.hpp>
#include <backend/models/Venue.hpp>
#include <util/logging/Log.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::VenuesController::VenuesController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace)
{
}

bool indiepub::VenuesController::insertVenue(const indiepub::Venue &venue)
{
    bool isValid = false;
    // Check if the venue_id is a valid UUID
    if (venue.venue_id().empty())
    {
        LOG_ERROR << "Venue ID cannot be empty";
        return isValid;
    }
    if (venue.owner_id().empty())
    {
        LOG_ERROR << "Owner ID cannot be empty";
        return isValid;
    }
    if (venue.name().empty())
    {
        LOG_ERROR << "Name cannot be empty";
        return isValid;
    }
    if (venue.location().empty())
    {
        LOG_ERROR << "Location cannot be empty";
        return isValid;
    }
    if (venue.capacity() <= 0)
    {
        LOG_ERROR << "Capacity must be positive";
        return isValid;
    }
    if (venue.created_at() <= 0)
    {
        LOG_ERROR << "Created at timestamp must be positive";
        return isValid;
    }

    // Check if the venue already exists
    indiepub::Venue existingVenue = getVenueBy(venue.name(), venue.location());
    if (existingVenue.venue_id() == venue.venue_id())
    {
        LOG_ERROR << "Venue with this name and location already exists";
        return isValid;
    }
    
    if (!isConnected())
    {
        LOG_ERROR << "Not connected to Cassandra";
        return isValid;
    }

    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY + " (venue_id, owner_id, name, location, capacity, created_at) VALUES (?, ?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 6);
    CassUuid venueId;
    if (cass_uuid_from_string(venue.venue_id().c_str(), &venueId) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + venue.venue_id();
    }
    CassUuid ownerId;
    if (cass_uuid_from_string(venue.owner_id().c_str(), &ownerId) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + venue.owner_id();
    }
    cass_statement_bind_uuid(statement, 0, venueId);
    cass_statement_bind_uuid(statement, 1, ownerId);
    cass_statement_bind_string(statement, 2, venue.name().c_str());
    cass_statement_bind_string(statement, 3, venue.location().c_str());
    cass_statement_bind_int32(statement, 4, venue.capacity());
    cass_statement_bind_int64(statement, 5, venue.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        LOG_ERROR << error_message;
    }
    else
    {
        isValid = true;
        std::cout << "Query executed successfully.";
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

bool indiepub::VenuesController::updateVenue(const indiepub::Venue &venue)
{
    bool isValid = false;
    // Check if the venue_id is a valid UUID
    if (venue.venue_id().empty())
    {
        LOG_ERROR << "Venue ID cannot be empty";
        return isValid;
    }
    if (venue.created_at() <= 0)
    {
        LOG_ERROR << "Created at timestamp must be positive";
        return isValid;
    }

    // Check if the venue already exists
    indiepub::Venue existingVenue = getVenueBy(venue.name(), venue.location());
    if (existingVenue.venue_id() == venue.venue_id())
    {
        LOG_ERROR << "Venue with this name and location already exists";
        return isValid;
    }
    
    if (!isConnected())
    {
        LOG_ERROR << "Not connected to Cassandra";
        return isValid;
    }

    std::string query = "UPDATE " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY + " SET owner_id=?, name=?, location=?, capacity=?  WHERE venue_id = ? AND created_at = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 6);
    CassUuid venueId;
    if (cass_uuid_from_string(venue.venue_id().c_str(), &venueId) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + venue.venue_id();
    }
    CassUuid ownerId;
    if (cass_uuid_from_string(venue.owner_id().c_str(), &ownerId) != CASS_OK)
    {
        LOG_ERROR << "Invalid UUID string: " + venue.owner_id();
    }
    
    cass_statement_bind_uuid(statement, 0, ownerId);
    cass_statement_bind_string(statement, 1, venue.name().c_str());
    cass_statement_bind_string(statement, 2, venue.location().c_str());
    cass_statement_bind_int32(statement, 3, venue.capacity());
    cass_statement_bind_uuid(statement, 4, venueId);
    cass_statement_bind_int64(statement, 5, venue.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        LOG_ERROR << error_message;
    }
    else
    {
        isValid = true;
        std::cout << "Query executed successfully.";
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::Venue> indiepub::VenuesController::getAllVenues()
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::Venue> venues;

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) == 0)
        {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator))
            {
                const CassRow *row = cass_iterator_get_row(iterator);
                venues.push_back(indiepub::Venue::from_row(row));
            }
            cass_iterator_free(iterator);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return venues;
}

indiepub::Venue indiepub::VenuesController::getVenueById(const std::string &venue_id)
{
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
    indiepub::Venue venue;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            venue = indiepub::Venue::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return venue;
}

indiepub::Venue indiepub::VenuesController::getVenueBy(const std::string &name, const std::string &location)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Venue::COLUMN_FAMILY + " WHERE name = ? AND location = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, location.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::Venue venue;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            venue = indiepub::Venue::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return venue;
}
