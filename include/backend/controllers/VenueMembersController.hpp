#ifndef INDIEPUB_VENUE_MEMBERS_CONTROLLER_HPP
#define INDIEPUB_VENUE_MEMBERS_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/VenueMembers.hpp>

namespace indiepub {

    class VenueMembersController : public CassandraConnection {
    public:
        VenueMembersController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertVenueMember(const indiepub::VenueMembers& member);
        bool updateVenueMember(const indiepub::VenueMembers& member);
        std::vector<indiepub::VenueMembers> getAllVenueMembers();
        indiepub::VenueMembers getVenueMemberById(const std::string& venue_id, const std::string& user_id);
        indiepub::VenueMembers getVenueMemberByUserId(const std::string &user_id);
        std::vector<indiepub::VenueMembers> getVenueMembersByRole(const std::string& role);
    
    private:
        // Add any private members or methods if needed
    };
}
#endif // INDIEPUB_VENUE_MEMBERS_CONTROLLER_HPP