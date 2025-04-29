#ifndef INDIEBACKCONTROLLER_HPP
#define INDIEBACKCONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/IndieBackModels.hpp>

namespace indiepub {

    class IndieBackController : public CassandraConnection {
    private:
        
        std::string keyspace_;

    public:
        IndieBackController() = default;
        IndieBackController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);
        // ~IndieBackController() = default;

        void insertUser(const indiepub::User& user);
        void insertVenue(const indiepub::Venue& venue);
        void insertBand(const indiepub::Band& band);
        void insertEvent(const indiepub::Event& event);
        void insertTicket(const indiepub::Ticket& ticket);
        void insertPost(const indiepub::Post& post);
        void insertDailyTicketSales(const indiepub::DailyTicketSales& sales);
 
        std::vector<indiepub::User> getAllUsers();
        std::vector<indiepub::Venue> getAllVenues();
        std::vector<indiepub::Band> getAllBands();
        std::vector<indiepub::Event> getAllEvents();
        std::vector<indiepub::Ticket> getAllTickets();
        std::vector<indiepub::Post> getAllPosts();
        std::vector<indiepub::DailyTicketSales> getAllDailyTicketSales();
/*    
        indiepub::User getUserById(const std::string& user_id);
        indiepub::Venue getVenueById(const std::string& venue_id);
        indiepub::Band getBandById(const std::string& band_id);
        indiepub::Event getEventById(const std::string& event_id);
        indiepub::Ticket getTicketById(const std::string& ticket_id);
        indiepub::Post getPostById(const std::string& post_id);

        indiepub::DailyTicketSales getDailyTicketSalesById(const std::string& event_id, const std::string& sale_date);
        std::vector<indiepub::Post> getPostsByUserId(const std::string& user_id);
        std::vector<indiepub::Ticket> getTicketsByUserId(const std::string& user_id);
        std::vector<indiepub::Ticket> getTicketsByEventId(const std::string& event_id);
        std::vector<indiepub::Event> getEventsByVenueId(const std::string& venue_id);
        std::vector<indiepub::Event> getEventsByBandId(const std::string& band_id);
        std::vector<indiepub::Event> getEventsByCreatorId(const std::string& creator_id);
        std::vector<indiepub::DailyTicketSales> getDailyTicketSalesByEventId(const std::string& event_id);
        std::vector<indiepub::DailyTicketSales> getDailyTicketSalesByDate(const std::string& sale_date);
        std::vector<indiepub::Band> getBandsByGenre(const std::string& genre);
        std::vector<indiepub::Venue> getVenuesByLocation(const std::string& location);
        std::vector<indiepub::BandMember> getBandMembersByBandId(const std::string& band_id);
        std::vector<indiepub::BandMember> getBandMembersByUserId(const std::string& user_id);
        
        void updateUser(const indiepub::User& user);
        void updateVenue(const indiepub::Venue& venue);
        void updateBand(const indiepub::Band& band);
        void updateEvent(const indiepub::Event& event);
        void updateTicket(const indiepub::Ticket& ticket);
        void updatePost(const indiepub::Post& post);
        void updateDailyTicketSales(const indiepub::DailyTicketSales& sales);
        
        void deleteUser(const std::string& user_id);
        void deleteVenue(const std::string& venue_id);
        void deleteBand(const std::string& band_id);
        void deleteEvent(const std::string& event_id);
        void deleteTicket(const std::string& ticket_id);
        void deletePost(const std::string& post_id);
        void deleteDailyTicketSales(const std::string& event_id, const std::string& sale_date);
        void deleteBandMember(const std::string& band_id, const std::string& user_id);
        
        void deleteAllUsers();
        void deleteAllVenues();
        void deleteAllBands();
        void deleteAllEvents();
        void deleteAllTickets();
        void deleteAllPosts();
        void deleteAllDailyTicketSales();
        void deleteAllBandMembers(); */

    };
}
#endif // INDIEBACKCONTROLLER_HPP