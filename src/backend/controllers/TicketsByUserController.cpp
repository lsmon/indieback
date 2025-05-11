#include <backend/controllers/TicketsByUserController.hpp>
#include <backend/models/TicketByUser.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::TicketsByUserController::TicketsByUserController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace)
{
    this->userController = std::make_shared<indiepub::UsersController>(contact_points, username, password, keyspace);
    this->eventController = std::make_shared<indiepub::EventController>(contact_points, username, password, keyspace);
}

bool indiepub::TicketsByUserController::insertTicket(const indiepub::TicketByUser &ticket)
{
    bool isValid = false;
    if (ticket.ticket_id().empty())
    {
        std::cerr << "Ticket ID cannot be empty" << std::endl;
        return isValid;
    }

    if (ticket.user_id().empty())
    {
        std::cerr << "User ID cannot be empty" << std::endl;
        return isValid;
    }

    if (ticket.event_id().empty())
    {
        std::cerr << "Event ID cannot be empty" << std::endl;
        return isValid;
    }

    if (ticket.purchase_date() <= 0)
    {
        std::cerr << "Purchase date must be positive" << std::endl;
        return isValid;
    }

    if (userController->getUserById(ticket.user_id()).user_id() != ticket.user_id())
    {
        std::cerr << "User with this ID does not exist" << std::endl;
        return isValid;
    }

    if (eventController->getEventById(ticket.event_id()).event_id() != ticket.event_id())
    {
        std::cerr << "Event with this ID does not exist" << std::endl;
        return isValid;
    }

    if (!isConnected())
    {
        std::cerr << "Not connected to Cassandra" << std::endl;
        return isValid;
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + indiepub::TicketByUser::COLUMN_FAMILY +
                        " (user_id, ticket_id, event_id, purchase_date) VALUES (?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid user_id;
    CassUuid ticket_id;
    CassUuid event_id;
    if (cass_uuid_from_string(ticket.user_id().c_str(), &user_id) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + ticket.user_id() << std::endl;
    }
    if (cass_uuid_from_string(ticket.ticket_id().c_str(), &ticket_id) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + ticket.ticket_id() << std::endl;
    }
    if (cass_uuid_from_string(ticket.event_id().c_str(), &event_id) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + ticket.event_id() << std::endl;
    }
    cass_statement_bind_uuid(statement, 0, user_id);
    cass_statement_bind_uuid(statement, 1, ticket_id);
    cass_statement_bind_uuid(statement, 2, event_id);
    cass_statement_bind_int64(statement, 3, ticket.purchase_date());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    else
    {
        std::cout << "Query executed successfully." << std::endl;
        isValid = true;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::TicketByUser> indiepub::TicketsByUserController::getAllTickets()
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByUser::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::TicketByUser> tickets;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.push_back(indiepub::TicketByUser::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return tickets;
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return tickets;
}

indiepub::TicketByUser indiepub::TicketsByUserController::getTicketById(const std::string &ticket_id)
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByUser::COLUMN_FAMILY + " WHERE ticket_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    indiepub::TicketByUser ticket;
    CassUuid uuid;
    if (cass_uuid_from_string(ticket_id.c_str(), &uuid) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + ticket_id << std::endl;
        return ticket;
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            ticket = indiepub::TicketByUser::from_row(row);
        }
        cass_iterator_free(cass_iterator_from_result(result));
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return ticket;
}

std::vector<indiepub::TicketByUser> indiepub::TicketsByUserController::getTicketsByUserId(const std::string &user_id)
{
    if (!isConnected())
    {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + indiepub::TicketByUser::IDX_TICKETS_EVENT_ID + " WHERE user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    std::vector<indiepub::TicketByUser> tickets;
    CassUuid uuid;
    if (cass_uuid_from_string(user_id.c_str(), &uuid) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + user_id << std::endl;
        return tickets;
    }
    cass_statement_bind_uuid(statement, 0, uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);

        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            tickets.push_back(indiepub::TicketByUser::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return tickets;
}
