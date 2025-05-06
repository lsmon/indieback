#include <backend/IndieBackController.hpp>
#include <backend/controllers/TicketsByEventController.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::TicketsByEventController::TicketsByEventController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::TicketsByEventController::insertTicket(const indiepub::TicketByEvent &ticket) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + indiepub::TicketByEvent::COLUMN_FAMILY +
                        " (event_id, ticket_id, user_id, date) VALUES (?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
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

std::vector<indiepub::TicketByEvent> indiepub::TicketsByEventController::getAllTickets() {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByEvent::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::TicketByEvent> tickets;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.emplace_back(indiepub::TicketByEvent::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return tickets;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

indiepub::TicketByEvent indiepub::TicketsByEventController::getTicketById(const std::string &ticket_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByEvent::COLUMN_FAMILY + " WHERE ticket_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(ticket_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + ticket_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            indiepub::TicketByEvent ticket = indiepub::TicketByEvent::from_row(row);
            cass_result_free(result);
            return ticket;
        } else {
            throw std::runtime_error("No ticket found with the given ID");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the ticket was not found
    throw std::runtime_error("Ticket not found");
}

std::vector<indiepub::TicketByEvent> indiepub::TicketsByEventController::getTicketsByUserId(const std::string &user_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByEvent::IDX_TICKETS_USER_ID + " WHERE user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::TicketByEvent> tickets;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.emplace_back(indiepub::TicketByEvent::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return tickets;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the ticket was not found
    throw std::runtime_error("Ticket not found");
}

std::vector<indiepub::TicketByEvent> indiepub::TicketsByEventController::getTicketsByEventId(const std::string &event_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByEvent::IDX_TICKETS_PURCHASE_DATE + " WHERE event_id = ?";
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
        std::vector<indiepub::TicketByEvent> tickets;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.emplace_back(indiepub::TicketByEvent::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return tickets;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the ticket was not found
    throw std::runtime_error("Ticket not found");
}
