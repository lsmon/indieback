#ifndef INDIEPUB_VENUE_MEMBERS_HPP
#define INDIEPUB_VENUE_MEMBERS_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub {
    class VenueMembers {
    public:
        VenueMembers() = default;
        VenueMembers(const std::string& venue_id, const std::string& user_id, const std::string& role,
                     std::time_t joined_at, bool is_active = true);

        static const std::string COLUMN_FAMILY;
        static const std::string PK_VENUE_ID;
        static const std::string CK_VENUE_USER_ID;
        static const std::string CK_JOINED_AT;
        static const std::string IDX_ROLE;
        static const std::string IDX_ACTIVE;

        // Getters
        std::string venue_id() const;
        std::string user_id() const;
        std::string role() const;
        std::time_t joined_at() const;
        bool is_active() const;

        // Setters
        void venue_id(const std::string& venue_id);
        void user_id(const std::string& user_id);
        void role(const std::string& role);
        void joined_at(const std::time_t& joined_at);
        void is_active(bool active);

        // JSON serialization
        std::string to_json() const;

        static VenueMembers from_json(const std::string& json);
        static VenueMembers from_row(const CassRow *row);

    private:
        std::string venue_id_;  // UUID of the venue
        std::string member_id_; // UUID of the member
        std::string role_;      // Role of the member in the venue
        std::time_t joined_at_; // Timestamp when the member joined the venue
        bool is_active_;        // Whether the member is currently active
    };
}


#endif // INDIEPUB_VENUE_MEMBERS_HPP