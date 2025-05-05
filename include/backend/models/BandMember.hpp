#ifndef INDIEPUB_BAND_MEMBER_HPP
#define INDIEPUB_BAND_MEMBER_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class BandMember
    {
    public:
        BandMember() = default;
        BandMember(const std::string &band_id, const std::string &user_id);

        // Getters
        std::string band_id() const;
        std::string user_id() const;

        // JSON serialization
        std::string to_json() const;

        static BandMember from_json(const std::string &json);
        static BandMember from_row(const CassRow *row);

    private:
        std::string band_id_; // UUID
        std::string user_id_; // UUID
    };
} // namespace indiepub

#endif // INDIEPUB_BAND_MEMBER_HPP
