#include <backend/controllers/EventController.hpp>
#include <backend/models/EventByVenue.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::EventController::EventController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::EventController::insertEvent(const indiepub::EventByVenue &event) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY + 
    " (venue_id, date, event_id, band_id, creator_id, name, price, capacity, sold) VALUES " + 
    " (?, ?, ?, ?, ?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 9);
    CassUuid venue_id;
    CassUuid event_id;
    CassUuid band_id;
    CassUuid creator_id;
    if (cass_uuid_from_string(event.venue_id().c_str(), &venue_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.venue_id());
    }
    if (cass_uuid_from_string(event.event_id().c_str(), &event_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.event_id());
    }
    if (cass_uuid_from_string(event.band_id().c_str(), &band_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.band_id());
    }
    if (cass_uuid_from_string(event.creator_id().c_str(), &creator_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event.creator_id());
    }
    cass_statement_bind_uuid(statement, 0, venue_id);
    cass_statement_bind_int64(statement, 1, event.date());
    cass_statement_bind_uuid(statement, 2, event_id);
    cass_statement_bind_uuid(statement, 3, band_id);
    cass_statement_bind_uuid(statement, 4, creator_id);
    cass_statement_bind_string(statement, 5, event.name().c_str());
    cass_statement_bind_int64(statement, 6, event.price());
    cass_statement_bind_int32(statement, 7, event.capacity());
    cass_statement_bind_int32(statement, 8, event.sold());

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

std::vector<indiepub::EventByVenue> indiepub::EventController::getAllEvents() {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::EventByVenue> events;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            events.emplace_back(indiepub::EventByVenue::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return events;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means no events were found
    throw std::runtime_error("No events found");
}

indiepub::EventByVenue indiepub::EventController::getEventById(const std::string &event_id) {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY + " WHERE event_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(event_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            indiepub::EventByVenue event = indiepub::EventByVenue::from_row(row);
            cass_result_free(result);
            return event;
        } else {
            throw std::runtime_error("No event found with the given ID");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the event was not found
    throw std::runtime_error("Event not found");
}

indiepub::EventByVenue indiepub::EventController::getEventBy(const std::string &name, const std::string &location) {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY + " WHERE name = ? AND location = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, location.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            indiepub::EventByVenue event = indiepub::EventByVenue::from_row(row);
            cass_result_free(result);
            return event;
        } else {
            throw std::runtime_error("No event found with the given name and location");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the event was not found
    throw std::runtime_error("Event not found");
}

