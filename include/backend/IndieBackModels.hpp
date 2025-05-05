#ifndef INDIEPUB_MODELS_HPP
#define INDIEPUB_MODELS_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub {

    // Helper function to convert timestamp to ISO 8601 string
    std::string timestamp_to_string(std::time_t time);

    // Helper function to parse ISO 8601 string to timestamp
    std::time_t string_to_timestamp(const std::string& str);

    
    // 2. Venues
    

    // 3. Bands
    

    // 4. Band Members
    
    // 5. Event (used for both events_by_venue and events_by_band)

    // 6. Ticket (used for both tickets_by_user and tickets_by_event)
    

    // 7. Post
    

    // 8. Daily Ticket Sales
    class DailyTicketSales {
    public:
        DailyTicketSales() = default;
        DailyTicketSales(const std::string& event_id, const std::string& sale_date, int tickets_sold);

        // Getters
        std::string event_id() const;
        std::string sale_date() const; // YYYY-MM-DD
        int tickets_sold() const;

        // JSON serialization
        std::string to_json() const;

        static DailyTicketSales from_json(const std::string& json);
        static DailyTicketSales from_row(const CassRow *row);
        
    private:
        std::string event_id_;      // UUID
        std::string sale_date_;     // YYYY-MM-DD
        int tickets_sold_;          // Counter
    };

} // namespace indiepub

#endif // INDIEPUB_MODELS_HPP