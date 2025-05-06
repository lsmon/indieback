#ifndef INDIEPUB_BAND_MEMBERS_CONTROLLER_HPP
#define INDIEPUB_BAND_MEMBERS_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/BandMember.hpp>

namespace indiepub {

    class BandMembersController : public CassandraConnection {
    public:
        BandMembersController() = default;
        BandMembersController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        void insertBandMember(const indiepub::BandMember& band_member);
        std::vector<indiepub::BandMember> getAllBandMembers();
        indiepub::BandMember getBandMemberById(const std::string& band_id, const std::string& user_id);
        std::vector<indiepub::BandMember> getBandMembersByBandId(const std::string& band_id);

    private:
        // Add any private members or methods if needed
    };

} // namespace indiepub

#endif // INDIEPUB_BAND_MEMBERS_CONTROLLER_HPP