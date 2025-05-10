#ifndef INDIEPUB_BAND_MEMBERS_CONTROLLER_HPP
#define INDIEPUB_BAND_MEMBERS_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/BandMember.hpp>
#include <backend/controllers/BandsController.hpp>
#include <backend/controllers/UsersController.hpp>
#include <memory>

namespace indiepub {

    class BandMembersController : public CassandraConnection {
    public:
        BandMembersController() = default;
        BandMembersController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertBandMember(const indiepub::BandMember& band_member);
        std::vector<indiepub::BandMember> getAllBandMembers();
        indiepub::BandMember getBandMemberById(const std::string& band_id, const std::string& user_id);
        std::vector<indiepub::BandMember> getBandMembersByBandId(const std::string& band_id);

    private:
        // Add any private members or methods if needed
        std::shared_ptr<UsersController> userController;
        std::shared_ptr<BandsController> bandController;
    };

} // namespace indiepub

#endif // INDIEPUB_BAND_MEMBERS_CONTROLLER_HPP