#include <backend/controllers/DailyTicketSalesController.hpp>
#include <backend/IndieBackModels.hpp>
#include <backend/models/DailyTicketSales.hpp>
#include <backend/models/EventByVenue.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

indiepub::DailyTicketSalesController::DailyTicketSalesController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace)
{
    eventController = std::make_shared<indiepub::EventController>(contact_points, username, password, keyspace);
}

bool indiepub::DailyTicketSalesController::insertDailyTicketSales(const indiepub::DailyTicketSales &daily_ticket_sales)
{
    bool isValid = false;
    // Check if the event_id is a valid UUID
    if (daily_ticket_sales.event_id().empty())
    {
        std::cerr << "Event ID cannot be empty" << std::endl;
        return isValid;
    }

    indiepub::EventByVenue _event = eventController->getEventById(daily_ticket_sales.event_id()); // Check if event exists
    if (_event.event_id() != daily_ticket_sales.event_id())
    {
        std::cerr << "Event with this ID does not exist" << std::endl;
        return isValid;
    }

    if (!isConnected())
    {
        std::cerr << "Not connected to Cassandra" << std::endl;
        return isValid;
    }

    std::string query = "UPDATE " + keyspace_ + "." + DailyTicketSales::COLUMN_FAMILY +
                        " SET tickets_sold = tickets_sold + 1 WHERE event_id = ? AND sale_date = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    CassUuid event_id;
    if (cass_uuid_from_string(daily_ticket_sales.event_id().c_str(), &event_id) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + daily_ticket_sales.event_id());
    }
    cass_statement_bind_uuid(statement, 0, event_id);
    cass_statement_bind_int64(statement, 1, daily_ticket_sales.sale_date());
    
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

std::vector<indiepub::DailyTicketSales> indiepub::DailyTicketSalesController::getAllDailyTicketSales()
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::DailyTicketSales::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::DailyTicketSales> daily_ticket_sales;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator))
            {
                const CassRow *row = cass_iterator_get_row(iterator);
                daily_ticket_sales.push_back(indiepub::DailyTicketSales::from_row(row));
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
        std::cerr << __FILE__ << ":" << __LINE__ << " : " << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return daily_ticket_sales;
}

indiepub::DailyTicketSales indiepub::DailyTicketSalesController::getDailyTicketSalesByEventId(const std::string &event_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::DailyTicketSales::COLUMN_FAMILY + " WHERE event_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid event_uuid;
    if (cass_uuid_from_string(event_id.c_str(), &event_uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + event_id);
    }
    cass_statement_bind_uuid(statement, 0, event_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::DailyTicketSales daily_ticket_sales; 
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            daily_ticket_sales = indiepub::DailyTicketSales::from_row(row);
        }
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
    return daily_ticket_sales;
}
