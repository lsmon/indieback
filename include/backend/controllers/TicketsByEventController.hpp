#ifndef INDIEPUB_TICKETS_BY_EVENT_CONTROLLER_HPP
#define INDIEPUB_TICKETS_BY_EVENT_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/TicketByEvent.hpp>

namespace indiepub {

    class TicketsByEventController : public CassandraConnection {
    public:
        TicketsByEventController() = default;
        TicketsByEventController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertTicket(const indiepub::TicketByEvent& ticket);
        std::vector<indiepub::TicketByEvent> getAllTickets();
        indiepub::TicketByEvent getTicketById(const std::string& ticket_id);
        std::vector<indiepub::TicketByEvent> getTicketsByUserId(const std::string& user_id);
        std::vector<indiepub::TicketByEvent> getTicketsByEventId(const std::string& event_id);

    private:
        // Add any private members or methods if needed
    };
}

#endif // INDIEPUB_TICKETS_CONTROLLER_HPP