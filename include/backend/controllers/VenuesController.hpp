#ifndef INDIEPUB_VENUES_CONTROLLER_HPP
#define INDIEPUB_VENUES_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/Venue.hpp>

namespace indiepub
{

    class VenuesController : public CassandraConnection
    {
    public:
#if __linux__
        VenuesController() = default;
#endif // __linux__
        VenuesController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace);

        bool insertVenue(const indiepub::Venue &venue);
        bool updateVenue(const indiepub::Venue &venue);
        std::vector<indiepub::Venue> getAllVenues();
        indiepub::Venue getVenueById(const std::string &venue_id);
        indiepub::Venue getVenueBy(const std::string &name, const std::string &location);

    private:
        // Add any private members or methods if needed
    };
}
#endif // INDIEPUB_VENUES_CONTROLLER_HPP