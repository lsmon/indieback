#ifndef INDIEPUB_DAILY_TICKET_SALES_HPP
#define INDIEPUB_DAILY_TICKET_SALES_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class DailyTicketSales
    {
    public:
        DailyTicketSales() = default;
        DailyTicketSales(const std::string &event_id, std::time_t sale_date, int tickets_sold);

        static const std::string COLUMN_FAMILY;

        // Getters
        std::string event_id() const;
        std::time_t sale_date() const; // YYYY-MM-DD
        int tickets_sold() const;

        // JSON serialization
        std::string to_json() const;

        static DailyTicketSales from_json(const std::string &json);
        static DailyTicketSales from_row(const CassRow *row);

    private:
        std::string event_id_;  // UUID
        std::time_t sale_date_; // YYYY-MM-DD
        int tickets_sold_;      // Counter
    };
}

#endif // INDIEPUB_DAILY_TICKET_SALES_HPP