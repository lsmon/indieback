#ifndef INDIEPUB_EVENT_BY_VENUE_HPP
#define INDIEPUB_EVENT_BY_VENUE_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class EventByVenue
    {
    public:
        EventByVenue() = default;
        EventByVenue(const std::string &event_id, const std::string &venue_id, const std::string &band_id,
              const std::string &creator_id, const std::string &name, std::time_t date,
              double price, int capacity, int sold);

        static const std::string COLUMN_FAMILY;
        
        static const std::string IDX_EVENTS_EVENT_ID;
        static const std::string IDX_EVENTS_BAND_ID;
        static const std::string IDX_EVENTS_CAPACITY;
        static const std::string IDX_EVENTS_CREATOR_ID;
        static const std::string IDX_EVENTS_NAME;
        static const std::string IDX_EVENTS_PRICE;
        static const std::string IDX_EVENTS_SOLD;
        
        // Getters
        std::string event_id() const;
        std::string venue_id() const;
        std::string band_id() const;
        std::string creator_id() const;
        std::string name() const;
        std::time_t date() const;
        double price() const;
        int capacity() const;
        int sold() const;

        // JSON serialization
        std::string to_json() const;

        static EventByVenue from_json(const std::string &json);
        static EventByVenue from_row(const CassRow *row);

    private:
        std::string event_id_;   // UUID
        std::string venue_id_;   // UUID
        std::string band_id_;    // UUID
        std::string creator_id_; // UUID
        std::string name_;
        std::time_t date_;
        double price_;
        int capacity_;
        int sold_;
    };
}

#endif // INDIEPUB_EVENT_BY_VENUE_HPP