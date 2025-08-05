#ifndef INDIEPUB_VENUE_HPP
#define INDIEPUB_VENUE_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{

    class Venue
    {
    public:
        Venue() = default;
        Venue(const std::string &venue_id, const std::string &owner_id, const std::string &name,
              const std::string &location, long capacity, std::time_t created_at);

        static const std::string COLUMN_FAMILY;
        static const std::string IDX_VENUES_NAME;
        static const std::string IDX_OWNERS_ID;
        static const std::string IDX_VENUES_LOCATION;
        // Getters
        std::string venue_id() const;
        std::string owner_id() const;
        std::string name() const;
        std::string location() const;
        long capacity() const;
        std::time_t created_at() const;

        // JSON serialization
        std::string to_json() const;

        static Venue from_json(const std::string &json);

        static Venue from_row(const CassRow *row);

    private:
        std::string venue_id_; // UUID
        std::string owner_id_; // UUID (links to User)
        std::string name_;
        std::string location_;
        long capacity_;
        std::time_t created_at_;
    };
}

#endif // INDIEPUB_VENUE_HPP