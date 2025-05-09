#include <backend/controllers/DailyTicketSalesController.hpp>
#include <backend/models/DailyTicketSales.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

indiepub::DailyTicketSalesController::DailyTicketSalesController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::DailyTicketSalesController::insertDailyTicketSales(const indiepub::DailyTicketSales &daily_ticket_sales) {
    // Check if the event_id is a valid UUID
    if (daily_ticket_sales.event_id().empty()) {
        throw std::runtime_error("Event ID cannot be empty");
    }
    if (daily_ticket_sales.sale_date().empty()) {
        throw std::runtime_error("Sale date cannot be empty");
    }
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::DailyTicketSales::COLUMN_FAMILY +
                        " (event_id, sale_date, tickets_sold) VALUES (?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 3);
    CassUuid event_id;
    if (cass_uuid_from_string(daily_ticket_sales.event_id().c_str(), &event_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + daily_ticket_sales.event_id());
    }
    cass_statement_bind_uuid(statement, 0, event_id);
    cass_statement_bind_string(statement, 1, daily_ticket_sales.sale_date().c_str());
    cass_statement_bind_int64(statement, 2, daily_ticket_sales.tickets_sold());

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

std::vector<indiepub::DailyTicketSales> indiepub::DailyTicketSalesController::getAllDailyTicketSales() {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::DailyTicketSales::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::DailyTicketSales> daily_ticket_sales;
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator)) {
                const CassRow *row = cass_iterator_get_row(iterator);
                daily_ticket_sales.push_back(indiepub::DailyTicketSales::from_row(row));
            }
            cass_iterator_free(iterator);
        }
        cass_result_free(result);
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return daily_ticket_sales;
}

indiepub::DailyTicketSales indiepub::DailyTicketSalesController::getDailyTicketSalesByEventId(const std::string &event_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::DailyTicketSales::COLUMN_FAMILY + " WHERE event_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid event_uuid;
    if (cass_uuid_from_string(event_id.c_str(), &event_uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + event_id);
    }
    cass_statement_bind_uuid(statement, 0, event_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_result_first_row(result);
            indiepub::DailyTicketSales daily_ticket_sales = indiepub::DailyTicketSales::from_row(row);
            cass_result_free(result);
            return daily_ticket_sales;
        } else {
            throw std::runtime_error("Daily ticket sales not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return indiepub::DailyTicketSales(); // Return an empty DailyTicketSales object if not found
}
