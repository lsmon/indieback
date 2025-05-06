#ifndef INDIEPUB_DAILY_TICKET_SALES_CONTROLLER_HPP
#define INDIEPUB_DAILY_TICKET_SALES_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/DailyTicketSales.hpp>

namespace indiepub {

    class DailyTicketSalesController : public CassandraConnection {
    public:
        DailyTicketSalesController() = default;
        DailyTicketSalesController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        void insertDailyTicketSales(const indiepub::DailyTicketSales& daily_ticket_sales);
        std::vector<indiepub::DailyTicketSales> getAllDailyTicketSales();
        indiepub::DailyTicketSales getDailyTicketSalesByEventId(const std::string& event_id);
    };

} // namespace indiepub

#endif // INDIEPUB_DAILY_TICKET_SALES_CONTROLLER_HPP