#ifndef INDIEPUB_EVENT_CONTROLLER_HPP
#define INDIEPUB_EVENT_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/EventByVenue.hpp>

namespace indiepub {

    class EventController : public CassandraConnection {
    public:
        EventController() = default;
        EventController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        void insertEvent(const indiepub::EventByVenue& event);
        std::vector<indiepub::EventByVenue> getAllEvents();
        indiepub::EventByVenue getEventById(const std::string& event_id);
        indiepub::EventByVenue getEventBy(const std::string& name, const std::string& location);

    private:
        // Add any private members or methods if needed
    };
}

#endif // INDIEPUB_EVENT_CONTROLLER_HPP