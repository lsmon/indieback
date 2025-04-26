
#include <memory>
#include <iostream>
#include <backend/IndieBackModels.hpp> 
#include <JSON.hpp>
#include <util/UUID.hpp>
#include <cassert>

std::unique_ptr<indiepub::User> user;
std::unique_ptr<indiepub::Venue> venue;
std::unique_ptr<indiepub::Band> band;
std::unique_ptr<indiepub::Event> event;
std::unique_ptr<indiepub::Ticket> ticket;
std::unique_ptr<indiepub::Post> post;
std::unique_ptr<indiepub::BandMember> bandMember;
std::unique_ptr<indiepub::DailyTicketSales> dailyTicketSales;

void testUserCRUD() {
    // Create a User object
    user = std::make_unique<indiepub::User>(UUID::random(), "abc@def.com", "fan", "John Doe", std::time(nullptr));
    std::cout << "User ID: " << user->user_id() << std::endl;
    std::cout << "Email: " << user->email() << std::endl;
    std::cout << "Role: " << user->role() << std::endl;
    std::cout << "Name: " << user->name() << std::endl;
    std::cout << "Created At: " << user->created_at() << std::endl;
    std::cout << "User JSON: " << user->to_json() << std::endl;
}

void testVenueCRUD() {
    // Create a Venue object
    venue = std::make_unique<indiepub::Venue>(UUID::random(), UUID::random(), "The Grand Hall", "123 Main St", 500, std::time(nullptr));
    std::cout << "Venue ID: " << venue->venue_id() << std::endl;
    std::cout << "Owner ID: " << venue->owner_id() << std::endl;
    std::cout << "Name: " << venue->name() << std::endl;
    std::cout << "Location: " << venue->location() << std::endl;
    std::cout << "Capacity: " << venue->capacity() << std::endl;
    std::cout << "Created At: " << venue->created_at() << std::endl;
    std::cout << "Venue JSON: " << venue->to_json() << std::endl;
}

void testBandCRUD() {
    // Create a Band object
    band = std::make_unique<indiepub::Band>(UUID::random(), "The Rockers", "Rock", "A popular rock band", std::time(nullptr));
    std::cout << "Band ID: " << band->band_id() << std::endl;
    std::cout << "Name: " << band->name() << std::endl;
    std::cout << "Genre: " << band->genre() << std::endl;
    std::cout << "Description: " << band->description() << std::endl;
    std::cout << "Created At: " << band->created_at() << std::endl;
    std::cout << "Band JSON: " << band->to_json() << std::endl;
}

void testEventCRUD() {
    // Create an Event object
    event = std::make_unique<indiepub::Event>(UUID::random(), UUID::random(), UUID::random(), UUID::random(), "Concert", std::time(nullptr), 50.0, 100, 10);
    std::cout << "Event ID: " << event->event_id() << std::endl;
    std::cout << "Venue ID: " << event->venue_id() << std::endl;
    std::cout << "Band ID: " << event->band_id() << std::endl;
    std::cout << "Creator ID: " << event->creator_id() << std::endl;
    std::cout << "Name: " << event->name() << std::endl;
    std::cout << "Date: " << event->date() << std::endl;
    std::cout << "Price: " << event->price() << std::endl;
    std::cout << "Capacity: " << event->capacity() << std::endl;
    std::cout << "Sold: " << event->sold() << std::endl;
    std::cout << "Event JSON: " << event->to_json() << std::endl;
}

void testTicketCRUD() {
    // Create a Ticket object
    ticket = std::make_unique<indiepub::Ticket>(UUID::random(), UUID::random(), UUID::random(), std::time(nullptr));
    std::cout << "Ticket ID: " << ticket->ticket_id() << std::endl;
    std::cout << "User ID: " << ticket->user_id() << std::endl;
    std::cout << "Event ID: " << ticket->event_id() << std::endl;
    std::cout << "Purchase Date: " << ticket->purchase_date() << std::endl;
    std::cout << "Ticket JSON: " << ticket->to_json() << std::endl;
}

void testPostCRUD() {
    // Create a Post object
    post = std::make_unique<indiepub::Post>(UUID::random(), UUID::random(), "This is a test post", std::time(nullptr), "2023-10-01");
    std::cout << "Post ID: " << post->post_id() << std::endl;
    std::cout << "User ID: " << post->user_id() << std::endl;
    std::cout << "Content: " << post->content() << std::endl;
    std::cout << "Created At: " << post->created_at() << std::endl;
    std::cout << "Date: " << post->date() << std::endl;
    std::cout << "Post JSON: " << post->to_json() << std::endl;
}

void testBandMemberCRUD() {
    // Create a BandMember object
    bandMember = std::make_unique<indiepub::BandMember>(UUID::random(), UUID::random());
    std::cout << "Band ID: " << bandMember->band_id() << std::endl;
    std::cout << "User ID: " << bandMember->user_id() << std::endl;
    std::cout << "Band Member JSON: " << bandMember->to_json() << std::endl;
}

void testDailyTicketSalesCRUD() {
    // Create a DailyTicketSales object
    dailyTicketSales = std::make_unique<indiepub::DailyTicketSales>(UUID::random(), indiepub::timestamp_to_string(std::time(nullptr)), 100);
    std::cout << "Event ID: " << dailyTicketSales->event_id() << std::endl;
    std::cout << "Date: " << dailyTicketSales->sale_date() << std::endl;
    std::cout << "Total Tickets: " << dailyTicketSales->tickets_sold() << std::endl;
    std::cout << "Daily Ticket Sales JSON: " << dailyTicketSales->to_json() << std::endl;
}

int main() {
    try
    {    
        testUserCRUD();
        testVenueCRUD();
        testBandCRUD();
        testEventCRUD();
        testTicketCRUD();
        testPostCRUD();
        testBandMemberCRUD();
        testDailyTicketSalesCRUD();
        assert(true);
    }
    catch(const std::exception& e)
    {
        assert(false);
        std::cerr << e.what() << '\n';
    }
    return 0;
}