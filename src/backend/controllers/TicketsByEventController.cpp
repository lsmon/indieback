#include <backend/controllers/TicketsByEventController.hpp>
#include <backend/models/TicketByEvent.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::TicketsByEventController::TicketsByEventController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
    this->userController = std::make_shared<indiepub::UsersController>(contact_points, username, password, keyspace);
    this->eventController = std::make_shared<indiepub::EventController>(contact_points, username, password, keyspace);
}

bool indiepub::TicketsByEventController::insertTicket(const indiepub::TicketByEvent &ticket) {
    bool isValid = false;

    if (ticket.ticket_id().empty()) {
        std::cerr << "Ticket ID cannot be empty" << std::endl;
        return isValid;
    }
    if (ticket.user_id().empty()) {
        std::cerr << "User ID cannot be empty" << std::endl;
        return isValid;
    }
    if (ticket.event_id().empty()) {
        std::cerr << "Event ID cannot be empty" << std::endl;
        return isValid;
    }
    if (ticket.purchase_date() <= 0) {
        std::cerr << "Purchase date must be positive" << std::endl;
        return isValid;
    }

    indiepub::TicketByEvent existingTicket = getTicketById(ticket.ticket_id()); // Check if ticket already exists
    if (existingTicket.ticket_id() == ticket.ticket_id()) {
        std::cerr << "Ticket with this ID already exists" << std::endl;
        return isValid;
    }

    indiepub::User _user = userController->getUserById(ticket.user_id()); // Check if user exists
    if (_user.user_id() != ticket.user_id()) {
        std::cerr << "User with this ID does not exist" << std::endl;
        return isValid;
    }
    indiepub::EventByVenue _event = eventController->getEventById(ticket.event_id()); // Check if event exists
    if (_event.event_id() != ticket.event_id()) {
        std::cerr << "Event with this ID does not exist" << std::endl;
        return isValid;
    }

    if (!isConnected()) {
        std::cerr << "Not connected to Cassandra" << std::endl;
        return isValid;
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + TicketByEvent::COLUMN_FAMILY +
                        " (event_id, ticket_id, user_id, purchase_date) VALUES (?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 4);
    CassUuid event_id;
    CassUuid ticket_id;
    CassUuid user_id;
    if (cass_uuid_from_string(ticket.event_id().c_str(), &event_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket.event_id());
    }
    if (cass_uuid_from_string(ticket.ticket_id().c_str(), &ticket_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket.ticket_id());
    }
    if (cass_uuid_from_string(ticket.user_id().c_str(), &user_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket.user_id());
    }
    cass_statement_bind_uuid(statement, 0, event_id);
    cass_statement_bind_uuid(statement, 1, ticket_id);
    cass_statement_bind_uuid(statement, 2, user_id);
    cass_statement_bind_int64(statement, 3, ticket.purchase_date());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) != CASS_OK) {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    } else {
        isValid = true;
        std::cout << "Ticket inserted successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::TicketByEvent> indiepub::TicketsByEventController::getAllTickets() {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + TicketByEvent::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::TicketByEvent> tickets;

    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.push_back(indiepub::TicketByEvent::from_row(row));
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

    return tickets;
}

indiepub::TicketByEvent indiepub::TicketsByEventController::getTicketById(const std::string &ticket_id) {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + TicketByEvent::COLUMN_FAMILY + " WHERE ticket_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(ticket_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::TicketByEvent ticket;

    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_result_first_row(result);
            ticket = indiepub::TicketByEvent::from_row(row);
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
    return ticket;
}

std::vector<indiepub::TicketByEvent> indiepub::TicketsByEventController::getTicketsByUserId(const std::string &user_id) {
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + TicketByEvent::COLUMN_FAMILY + " WHERE user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::TicketByEvent> tickets;

    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);    
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.push_back(indiepub::TicketByEvent::from_row(row));
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
    return tickets;
}

std::vector<indiepub::TicketByEvent> indiepub::TicketsByEventController::getTicketsByEventId(const std::string &event_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + TicketByEvent::COLUMN_FAMILY + " WHERE event_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(event_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::TicketByEvent> tickets;

    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        
        CassIterator *iterator = cass_iterator_from_result(result);
        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.emplace_back(indiepub::TicketByEvent::from_row(row));
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
    return tickets;
}
