#include <backend/controllers/EventController.hpp>
#include <backend/models/EventByVenue.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::EventController::EventController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
}

bool indiepub::EventController::insertEvent(const indiepub::EventByVenue &event) {
    bool isValid = false;
    // Check if the event_id is a valid UUID
    if (event.event_id().empty()) {
        std::cerr << "Event ID cannot be empty" << std::endl;
        return isValid;
    }
    if (event.venue_id().empty()) {
        std::cerr << "Venue ID cannot be empty" << std::endl;
        return isValid;
    }
    if (event.band_id().empty()) {
        std::cerr << "Band ID cannot be empty" << std::endl;
        return isValid;
    }
    if (event.creator_id().empty()) {
        std::cerr << "Creator ID cannot be empty" << std::endl;
        return isValid;
    }
    if (event.name().empty()) {
        std::cerr << "Name cannot be empty" << std::endl;
        return isValid;
    }
    if (event.date() <= 0) {
        std::cerr << "Date must be positive" << std::endl;
        return isValid;
    }
    if (event.price() < 0) {
        std::cerr << "Price cannot be negative" << std::endl;
        return isValid;
    }
    if (event.capacity() <= 0) {
        std::cerr << "Capacity must be positive" << std::endl;
        return isValid;
    }
    if (event.sold() < 0) {
        std::cerr << "Sold tickets cannot be negative" << std::endl;
        return isValid;
    }

    indiepub::EventByVenue existingEvent = getEventById(event.event_id());
    if (existingEvent.event_id() == event.event_id()) {
        std::cerr << "Event with this ID already exists" << std::endl;
        return isValid;
    }
    
    indiepub::EventByVenue existingEventByNameAndVenue = getEventBy(event.name(), event.venue_id());
    if (existingEventByNameAndVenue.event_id() == event.event_id()) {
        std::cerr << "Event with this name and venue already exists" << std::endl;
        return isValid;
    }

    if (!isConnected()) {
        std::cerr << "Not connected to Cassandra" << std::endl;
        return isValid;
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
        std::cerr << "Invalid UUID string: " + event.venue_id() << std::endl;
    }
    if (cass_uuid_from_string(event.event_id().c_str(), &event_id) != CASS_OK) {
        std::cerr << "Invalid UUID string: " + event.event_id() << std::endl;
    }
    if (cass_uuid_from_string(event.band_id().c_str(), &band_id) != CASS_OK) {
        std::cerr << "Invalid UUID string: " + event.band_id() << std::endl;
    }
    if (cass_uuid_from_string(event.creator_id().c_str(), &creator_id) != CASS_OK) {
        std::cerr << "Invalid UUID string: " + event.creator_id() << std::endl;
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << error_message << std::endl;
    } else {
        std::cout << "Query executed successfully." << std::endl;
        isValid = true;
    }
    
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::EventByVenue> indiepub::EventController::getAllEvents() {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::EventByVenue> events;
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            events.emplace_back(indiepub::EventByVenue::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
    } else {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means no events were found
    return events;
}

std::vector<indiepub::EventByVenue> indiepub::EventController::getOneWeekEvents(const time_t &start_date) {
    std::vector<indiepub::EventByVenue> events;
    time_t end_date = start_date + 7 * 24 * 60 * 60; // One week later
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY + 
                        " WHERE date >= ? AND date <= ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_int64(statement, 0, start_date);
    cass_statement_bind_int64(statement, 1, end_date);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        CassIterator *iterator = cass_iterator_from_result(result);
        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            events.emplace_back(indiepub::EventByVenue::from_row(row));
        }
        cass_iterator_free(iterator);
        cass_result_free(result);
    } else {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means no events were found
    return events;
}



indiepub::EventByVenue indiepub::EventController::getEventById(const std::string &event_id) {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY + " WHERE event_id = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    indiepub::EventByVenue event;

    if (cass_uuid_from_string(event_id.c_str(), &uuid) != CASS_OK) {
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Invalid UUID string: " + event_id << std::endl;
        return event; // Return an empty Event object in case of failure
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_result_first_row(result);
            event = indiepub::EventByVenue::from_row(row);
        } 
        cass_result_free(result);
    } else {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the event was not found
    return event;
}

indiepub::EventByVenue indiepub::EventController::getEventBy(const std::string &name, const std::string &venue_id) {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + EventByVenue::COLUMN_FAMILY + " WHERE venue_id = ? AND name = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    CassUuid uuid;
    indiepub::EventByVenue event;

    if (cass_uuid_from_string(venue_id.c_str(), &uuid) != CASS_OK) {
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Invalid UUID string: " + venue_id << std::endl;
        return event; // Return an empty Event object in case of failure
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    cass_statement_bind_string(statement, 1, name.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_result_first_row(result);
            event = indiepub::EventByVenue::from_row(row);
        }
        cass_result_free(result);
    } else {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the event was not found
    return event;
}

