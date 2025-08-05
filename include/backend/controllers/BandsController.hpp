#ifndef INDIEPUB_BANDS_CONTROLLER_HPP
#define INDIEPUB_BANDS_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/Band.hpp>

namespace indiepub {

    class BandsController : public CassandraConnection {
    public:
#if __linux__
        BandsController() = default;
#endif // __linux__
        BandsController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertBand(const indiepub::Band& band);
        std::vector<indiepub::Band> getAllBands();
        indiepub::Band getBandById(const std::string& band_id);
        indiepub::Band getBandByName(const std::string& name);
        indiepub::Band getBandBy(const std::string& name, const std::string& genre);
        
    private:
        // Add any private members or methods if needed
    };
}

#endif // INDIEPUB_BANDS_CONTROLLER_HPP