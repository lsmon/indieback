#ifndef INDIEPUB_TICKETS_BY_USER_CONTROLLER_HPP
#define INDIEPUB_TICKETS_BY_USER_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/TicketByUser.hpp>
#include <backend/controllers/EventController.hpp>
#include <backend/controllers/UsersController.hpp>
#include <memory>

namespace indiepub {

    class TicketsByUserController : public CassandraConnection {
    public:
        TicketsByUserController() = default;
        TicketsByUserController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertTicket(const indiepub::TicketByUser& ticket);
        std::vector<indiepub::TicketByUser> getAllTickets();
        indiepub::TicketByUser getTicketById(const std::string& ticket_id);
        std::vector<indiepub::TicketByUser> getTicketsByUserId(const std::string& user_id);

    private:
        // Add any private members or methods if needed
        std::shared_ptr<UsersController> userController;
        std::shared_ptr<EventController> eventController;
    };
}

#endif // INDIEPUB_TICKETS_BY_USER_CONTROLLER_HPP