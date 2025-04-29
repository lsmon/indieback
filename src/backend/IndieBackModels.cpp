#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

std::string indiepub::timestamp_to_string(std::time_t time) {
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&time));
    return buffer;
}

std::time_t indiepub::string_to_timestamp(const std::string& str) {
    std::tm tm = {};
    strptime(str.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return std::mktime(&tm);
}

indiepub::User::User(const std::string& user_id, const std::string& email, const std::string& role,
                      const std::string& name, std::time_t created_at)
    : user_id_(user_id), email_(email), role_(role), name_(name), created_at_(created_at) {}

std::string indiepub::User::user_id() const {
    return user_id_;
}

std::string indiepub::User::email() const {
    return email_;
}

std::string indiepub::User::role() const {
    return role_;
}

std::string indiepub::User::name() const {
    return name_;
}

std::time_t indiepub::User::created_at() const {
    return created_at_;
}

std::string indiepub::User::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("user_id", user_id_);
    json->put("email", email_);
    json->put("role", role_);
    json->put("name", name_);
    json->put("created_at", timestamp_to_string(created_at_));
    return json->str();
    
}

indiepub::User indiepub::User::from_json(const std::string& json) {
    User user;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    user.user_id_ = jsonObject->get("user_id").str();
    user.email_ = jsonObject->get("email").str();
    user.role_ = jsonObject->get("role").str();
    user.name_ = jsonObject->get("name").str();
    user.created_at_ = string_to_timestamp(jsonObject->get("created_at").str());
    return user;
}

indiepub::User indiepub::User::from_row(const CassRow *row)
{
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &user_id, &user_id_length);
    
        const char *email;
        size_t email_length;
        cass_value_get_string(cass_row_get_column(row, 1), &email, &email_length);
    
        const char *role;
        size_t role_length;
        cass_value_get_string(cass_row_get_column(row, 2), &role, &role_length);
    
        const char *name;
        size_t name_length;
        cass_value_get_string(cass_row_get_column(row, 3), &name, &name_length);
    
        cass_int64_t created_at;
        cass_value_get_int64(cass_row_get_column(row, 4), &created_at);
        return User(std::string(user_id, user_id_length),
                            std::string(email, email_length),
                            std::string(role, role_length),
                            std::string(name, name_length),
                            created_at);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return User();
    }
}

indiepub::Venue::Venue(const std::string &venue_id, const std::string &owner_id, const std::string &name,
                       const std::string &location, int capacity, std::time_t created_at)
    : venue_id_(venue_id), owner_id_(owner_id), name_(name), location_(location),
      capacity_(capacity), created_at_(created_at) {}

std::string indiepub::Venue::venue_id() const {
    return venue_id_;
}

std::string indiepub::Venue::owner_id() const {
    return owner_id_;
}

std::string indiepub::Venue::name() const {
    return name_;
}

std::string indiepub::Venue::location() const {
    return location_;
}

int indiepub::Venue::capacity() const {
    return capacity_;
}

std::time_t indiepub::Venue::created_at() const {
    return created_at_;
}

std::string indiepub::Venue::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("venue_id", venue_id_);
    json->put("owner_id", owner_id_);
    json->put("name", name_);
    json->put("location", location_);
    json->put("capacity", capacity_);
    json->put("created_at", timestamp_to_string(created_at_));
    return json->str();
}

indiepub::Venue indiepub::Venue::from_json(const std::string& json) {
    Venue venue;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    venue.venue_id_ = jsonObject->get("venue_id").str();
    venue.owner_id_ = jsonObject->get("owner_id").str();
    venue.name_ = jsonObject->get("name").str();
    venue.location_ = jsonObject->get("location").str();
    venue.capacity_ = std::stoi(jsonObject->get("capacity").str());
    venue.created_at_ = string_to_timestamp(jsonObject->get("created_at").str());
    return venue;
}

indiepub::Venue indiepub::Venue::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *venue_id;
        size_t venue_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &venue_id, &venue_id_length);
    
        const char *owner_id;
        size_t owner_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &owner_id, &owner_id_length);
    
        const char *name;
        size_t name_length;
        cass_value_get_string(cass_row_get_column(row, 2), &name, &name_length);
    
        const char *location;
        size_t location_length;
        cass_value_get_string(cass_row_get_column(row, 3), &location, &location_length);
    
        int capacity;
        cass_value_get_int32(cass_row_get_column(row, 4), &capacity);
    
        cass_int64_t created_at;
        cass_value_get_int64(cass_row_get_column(row, 5), &created_at);
    
        return Venue(std::string(venue_id, venue_id_length),
                     std::string(owner_id, owner_id_length),
                     std::string(name, name_length),
                     std::string(location, location_length),
                     capacity,
                     created_at);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return Venue();
    }
}

indiepub::Band::Band(const std::string& band_id, const std::string& name,
                      const std::string& genre, const std::string& description, std::time_t created_at)
    : band_id_(band_id), name_(name), genre_(genre),
      description_(description), created_at_(created_at) {}

std::string indiepub::Band::band_id() const {
    return band_id_;
}

std::string indiepub::Band::name() const {
    return name_;
}

std::string indiepub::Band::genre() const {
    return genre_;
}

std::string indiepub::Band::description() const {
    return description_;
}

std::time_t indiepub::Band::created_at() const {
    return created_at_;
}

std::string indiepub::Band::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("band_id", band_id_);
    json->put("name", name_);
    json->put("genre", genre_);
    json->put("description", description_);
    json->put("created_at", timestamp_to_string(created_at_));
    return json->str();
}

indiepub::Band indiepub::Band::from_json(const std::string& json) {
    Band band;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    band.band_id_ = jsonObject->get("band_id").str();
    band.name_ = jsonObject->get("name").str();
    band.genre_ = jsonObject->get("genre").str();
    band.description_ = jsonObject->get("description").str();
    band.created_at_ = string_to_timestamp(jsonObject->get("created_at").str());
    return band;
}

indiepub::Band indiepub::Band::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *band_id;
        size_t band_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &band_id, &band_id_length);
    
        const char *name;
        size_t name_length;
        cass_value_get_string(cass_row_get_column(row, 1), &name, &name_length);
    
        const char *genre;
        size_t genre_length;
        cass_value_get_string(cass_row_get_column(row, 2), &genre, &genre_length);
    
        const char *description;
        size_t description_length;
        cass_value_get_string(cass_row_get_column(row, 3), &description, &description_length);
    
        cass_int64_t created_at;
        cass_value_get_int64(cass_row_get_column(row, 4), &created_at);
    
        return Band(std::string(band_id, band_id_length),
                    std::string(name, name_length),
                    std::string(genre, genre_length),
                    std::string(description, description_length),
                    created_at);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return Band();
    }
}

indiepub::BandMember::BandMember(const std::string& band_id, const std::string& user_id)
    : band_id_(band_id), user_id_(user_id) {}

std::string indiepub::BandMember::band_id() const {
    return band_id_;
}

std::string indiepub::BandMember::user_id() const {
    return user_id_;
}

std::string indiepub::BandMember::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("band_id", band_id_);
    json->put("user_id", user_id_);
    return json->str();
}

indiepub::BandMember indiepub::BandMember::from_json(const std::string& json) {
    BandMember member;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    member.band_id_ = jsonObject->get("band_id").str();
    member.user_id_ = jsonObject->get("user_id").str();
    return member;
}

indiepub::BandMember indiepub::BandMember::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *band_id;
        size_t band_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &band_id, &band_id_length);
    
        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &user_id, &user_id_length);
    
        return BandMember(std::string(band_id, band_id_length),
                          std::string(user_id, user_id_length));
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return BandMember();
    }
}

indiepub::Event::Event(const std::string& event_id, const std::string& venue_id,
                        const std::string& band_id, const std::string& creator_id,
                        const std::string& name, std::time_t date, double price,
                        int capacity, int sold)
    : event_id_(event_id), venue_id_(venue_id), band_id_(band_id),
      creator_id_(creator_id), name_(name), date_(date),
      price_(price), capacity_(capacity), sold_(sold) {}

std::string indiepub::Event::event_id() const {
    return event_id_;
}

std::string indiepub::Event::venue_id() const {
    return venue_id_;
}

std::string indiepub::Event::band_id() const {
    return band_id_;
}

std::string indiepub::Event::creator_id() const {
    return creator_id_;
}

std::string indiepub::Event::name() const {
    return name_;
}

std::time_t indiepub::Event::date() const {
    return date_;
}

double indiepub::Event::price() const {
    return price_;
}

int indiepub::Event::capacity() const {
    return capacity_;
}

int indiepub::Event::sold() const {
    return sold_;
}

std::string indiepub::Event::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("event_id", event_id_);
    json->put("venue_id", venue_id_);
    json->put("band_id", band_id_);
    json->put("creator_id", creator_id_);
    json->put("name", name_);
    json->put("date", timestamp_to_string(date_));
    json->put("price", price_);
    json->put("capacity", capacity_);
    json->put("sold", sold_);
    return json->str();
}

indiepub::Event indiepub::Event::from_json(const std::string& json) {
    Event event;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    event.event_id_ = jsonObject->get("event_id").str();
    event.venue_id_ = jsonObject->get("venue_id").str();
    event.band_id_ = jsonObject->get("band_id").str();
    event.creator_id_ = jsonObject->get("creator_id").str();
    event.name_ = jsonObject->get("name").str();
    event.date_ = string_to_timestamp(jsonObject->get("date").str());
    event.price_ = std::stod(jsonObject->get("price").str());
    event.capacity_ = std::stoi(jsonObject->get("capacity").str());
    event.sold_ = std::stoi(jsonObject->get("sold").str());
    return event;
}

indiepub::Event indiepub::Event::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *event_id;
        size_t event_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &event_id, &event_id_length);
    
        const char *venue_id;
        size_t venue_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &venue_id, &venue_id_length);
    
        const char *band_id;
        size_t band_id_length;
        cass_value_get_string(cass_row_get_column(row, 2), &band_id, &band_id_length);
    
        const char *creator_id;
        size_t creator_id_length;
        cass_value_get_string(cass_row_get_column(row, 3), &creator_id, &creator_id_length);
    
        const char *name;
        size_t name_length;
        cass_value_get_string(cass_row_get_column(row, 4), &name, &name_length);
    
        cass_int64_t date;
        cass_value_get_int64(cass_row_get_column(row, 5), &date);
    
        double price;
        cass_value_get_double(cass_row_get_column(row, 6), &price);
    
        int capacity;
        cass_value_get_int32(cass_row_get_column(row, 7), &capacity);
    
        int sold;
        cass_value_get_int32(cass_row_get_column(row, 8), &sold);
    
        return Event(std::string(event_id, event_id_length),
                     std::string(venue_id, venue_id_length),
                     std::string(band_id, band_id_length),
                     std::string(creator_id, creator_id_length),
                     std::string(name, name_length),
                     date,
                     price,
                     capacity,
                     sold);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return Event();
    }
}

indiepub::Ticket::Ticket(const std::string& ticket_id, const std::string& user_id,
                            const std::string& event_id, std::time_t purchase_date)
    : ticket_id_(ticket_id), user_id_(user_id), event_id_(event_id),
      purchase_date_(purchase_date) {}

std::string indiepub::Ticket::ticket_id() const {
    return ticket_id_;
}

std::string indiepub::Ticket::user_id() const {
    return user_id_;
}

std::string indiepub::Ticket::event_id() const {
    return event_id_;
}

std::time_t indiepub::Ticket::purchase_date() const {
    return purchase_date_;
}

std::string indiepub::Ticket::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("ticket_id", ticket_id_);
    json->put("user_id", user_id_);
    json->put("event_id", event_id_);
    json->put("purchase_date", timestamp_to_string(purchase_date_));
    return json->str();
}

indiepub::Ticket indiepub::Ticket::from_json(const std::string& json) {
    Ticket ticket;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    ticket.ticket_id_ = jsonObject->get("ticket_id").str();
    ticket.user_id_ = jsonObject->get("user_id").str();
    ticket.event_id_ = jsonObject->get("event_id").str();
    ticket.purchase_date_ = string_to_timestamp(jsonObject->get("purchase_date").str());
    return ticket;
}

indiepub::Ticket indiepub::Ticket::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *ticket_id;
        size_t ticket_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &ticket_id, &ticket_id_length);
    
        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &user_id, &user_id_length);
    
        const char *event_id;
        size_t event_id_length;
        cass_value_get_string(cass_row_get_column(row, 2), &event_id, &event_id_length);
    
        cass_int64_t purchase_date;
        cass_value_get_int64(cass_row_get_column(row, 3), &purchase_date);
    
        return Ticket(std::string(ticket_id, ticket_id_length),
                      std::string(user_id, user_id_length),
                      std::string(event_id, event_id_length),
                      purchase_date);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return Ticket();
    }
}

indiepub::Post::Post(const std::string& post_id, const std::string& user_id,
                      const std::string& content, std::time_t created_at, const std::string& date)
    : post_id_(post_id), user_id_(user_id), content_(content),
      created_at_(created_at), date_(date) {}

std::string indiepub::Post::post_id() const {
    return post_id_;
}

std::string indiepub::Post::user_id() const {
    return user_id_;
}

std::string indiepub::Post::content() const {
    return content_;
}

std::time_t indiepub::Post::created_at() const {
    return created_at_;
}

std::string indiepub::Post::date() const {
    return date_;
}

std::string indiepub::Post::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("post_id", post_id_);
    json->put("user_id", user_id_);
    json->put("content", content_);
    json->put("created_at", timestamp_to_string(created_at_));
    json->put("date", date_);
    return json->str();
}

indiepub::Post indiepub::Post::from_json(const std::string& json) {
    Post post;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    post.post_id_ = jsonObject->get("post_id").str();
    post.user_id_ = jsonObject->get("user_id").str();
    post.content_ = jsonObject->get("content").str();
    post.created_at_ = string_to_timestamp(jsonObject->get("created_at").str());
    post.date_ = jsonObject->get("date").str();
    return post;
}

indiepub::Post indiepub::Post::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *post_id;
        size_t post_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &post_id, &post_id_length);
    
        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &user_id, &user_id_length);
    
        const char *content;
        size_t content_length;
        cass_value_get_string(cass_row_get_column(row, 2), &content, &content_length);
    
        cass_int64_t created_at;
        cass_value_get_int64(cass_row_get_column(row, 3), &created_at);
    
        const char *date;
        size_t date_length;
        cass_value_get_string(cass_row_get_column(row, 4), &date, &date_length);
    
        return Post(std::string(post_id, post_id_length),
                    std::string(user_id, user_id_length),
                    std::string(content, content_length),
                    created_at,
                    std::string(date, date_length));
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return Post();
    }
}

indiepub::DailyTicketSales::DailyTicketSales(const std::string& event_id, const std::string& sale_date, int tickets_sold)
    : event_id_(event_id), sale_date_(sale_date), tickets_sold_(tickets_sold) {}

std::string indiepub::DailyTicketSales::event_id() const {
    return event_id_;
}

std::string indiepub::DailyTicketSales::sale_date() const {
    return sale_date_;
}

int indiepub::DailyTicketSales::tickets_sold() const {
    return tickets_sold_;
}

std::string indiepub::DailyTicketSales::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("event_id", event_id_);
    json->put("sale_date", sale_date_);
    json->put("tickets_sold", tickets_sold_);
    return json->str();
}

indiepub::DailyTicketSales indiepub::DailyTicketSales::from_json(const std::string& json) {
    DailyTicketSales sales;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    sales.event_id_ = jsonObject->get("event_id").str();
    sales.sale_date_ = jsonObject->get("sale_date").str();
    sales.tickets_sold_ = std::stoi(jsonObject->get("tickets_sold").str());
    return sales;
}

indiepub::DailyTicketSales indiepub::DailyTicketSales::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *event_id;
        size_t event_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &event_id, &event_id_length);
    
        const char *sale_date;
        size_t sale_date_length;
        cass_value_get_string(cass_row_get_column(row, 1), &sale_date, &sale_date_length);
    
        int tickets_sold;
        cass_value_get_int32(cass_row_get_column(row, 2), &tickets_sold);
    
        return DailyTicketSales(std::string(event_id, event_id_length),
                                std::string(sale_date, sale_date_length),
                                tickets_sold);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return DailyTicketSales();
    }
}