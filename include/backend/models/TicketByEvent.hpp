#ifndef INDIEPUB_TICKET_BY_EVENT_HPP
#define INDIEPUB_TICKET_BY_EVENT_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class TicketByEvent
    {
    public:
        TicketByEvent() = default;
        TicketByEvent(const std::string &ticket_id, const std::string &user_id, const std::string &event_id,
               std::time_t purchase_date);

        static const std::string COLUMN_FAMILY;
        static const std::string IDX_TICKETS_USER_ID;
        static const std::string IDX_TICKETS_PURCHASE_DATE;

        // Getters
        std::string ticket_id() const;
        std::string user_id() const;
        std::string event_id() const;
        std::time_t purchase_date() const;

        // JSON serialization
        std::string to_json() const;

        static TicketByEvent from_json(const std::string &json);
        static TicketByEvent from_row(const CassRow *row);

    private:
        std::string ticket_id_; // UUID
        std::string user_id_;   // UUID
        std::string event_id_;  // UUID
        std::time_t purchase_date_;
    };
}
#endif // INDIEPUB_TICKET_HPP