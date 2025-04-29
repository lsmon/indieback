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

    // 1. Users
    class User {
    public:
        User() = default;
        User(const std::string& user_id, const std::string& email, const std::string& role,
             const std::string& name, std::time_t created_at);

        // Getters
        std::string user_id() const;
        std::string email() const;
        std::string role() const;
        std::string name() const;
        std::time_t created_at() const;

        // JSON serialization
        std::string to_json() const;

        static User from_json(const std::string& json);

        static User from_row(const CassRow *row);

    private:
        std::string user_id_;  // UUID
        std::string email_;
        std::string role_;    // 'fan', 'band', 'venue'
        std::string name_;
        std::time_t created_at_;
    };

    // 2. Venues
    class Venue {
    public:
        Venue() = default;
        Venue(const std::string& venue_id, const std::string& owner_id, const std::string& name,
              const std::string& location, int capacity, std::time_t created_at);

        // Getters
        std::string venue_id() const;
        std::string owner_id() const;
        std::string name() const;
        std::string location() const;
        int capacity() const;
        std::time_t created_at() const;

        // JSON serialization
        std::string to_json() const;

        static Venue from_json(const std::string& json);

        static Venue from_row(const CassRow *row);

    private:
        std::string venue_id_;  // UUID
        std::string owner_id_;  // UUID (links to User)
        std::string name_;
        std::string location_;
        int capacity_;
        std::time_t created_at_;
    };

    // 3. Bands
    class Band {
    public:
        Band() = default;
        Band(const std::string& band_id, const std::string& name, const std::string& genre,
             const std::string& description, std::time_t created_at);

        // Getters
        std::string band_id() const;
        std::string name() const;
        std::string genre() const;
        std::string description() const;
        std::time_t created_at() const;

        // JSON serialization
        std::string to_json() const;

        static Band from_json(const std::string& json);
        static Band from_row(const CassRow *row);

    private:
        std::string band_id_;  // UUID
        std::string name_;
        std::string genre_;
        std::string description_;
        std::time_t created_at_;
    };

    // 4. Band Members
    class BandMember {
    public:
        BandMember() = default;
        BandMember(const std::string& band_id, const std::string& user_id);

        // Getters
        std::string band_id() const;
        std::string user_id() const;

        // JSON serialization
        std::string to_json() const;

        static BandMember from_json(const std::string& json);
        static BandMember from_row(const CassRow *row);

    private:
        std::string band_id_;  // UUID
        std::string user_id_;  // UUID
    };

    // 5. Event (used for both events_by_venue and events_by_band)
    class Event {
    public:
        Event() = default;
        Event(const std::string& event_id, const std::string& venue_id, const std::string& band_id,
              const std::string& creator_id, const std::string& name, std::time_t date,
              double price, int capacity, int sold);
              
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

        static Event from_json(const std::string& json);
        static Event from_row(const CassRow *row);

    private:
        std::string event_id_;    // UUID
        std::string venue_id_;    // UUID
        std::string band_id_;     // UUID
        std::string creator_id_;  // UUID
        std::string name_;
        std::time_t date_;
        double price_;
        int capacity_;
        int sold_;
    };

    // 6. Ticket (used for both tickets_by_user and tickets_by_event)
    class Ticket {
    public:
        Ticket() = default;
        Ticket(const std::string& ticket_id, const std::string& user_id, const std::string& event_id,
               std::time_t purchase_date);

        // Getters
        std::string ticket_id() const;
        std::string user_id() const;
        std::string event_id() const;
        std::time_t purchase_date() const;

        // JSON serialization
        std::string to_json() const;

        static Ticket from_json(const std::string& json);
        static Ticket from_row(const CassRow *row);

    private:
        std::string ticket_id_;      // UUID
        std::string user_id_;        // UUID
        std::string event_id_;       // UUID
        std::time_t purchase_date_;
    };

    // 7. Post
    class Post {
    public:
        Post() = default;
        Post(const std::string& post_id, const std::string& user_id, const std::string& content,
             std::time_t created_at, const std::string& date);

        // Getters
        std::string post_id() const;
        std::string user_id() const;
        std::string content() const;
        std::time_t created_at() const;
        std::string date() const; // Date as YYYY-MM-DD

        // JSON serialization
        std::string to_json() const;

        static Post from_json(const std::string& json);
        static Post from_row(const CassRow *row);

    private:
        std::string post_id_;    // UUID
        std::string user_id_;    // UUID
        std::string content_;
        std::time_t created_at_;
        std::string date_;       // YYYY-MM-DD
    };

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