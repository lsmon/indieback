#include <backend/models/Venue.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

indiepub::Venue::Venue(const std::string &venue_id, const std::string &owner_id, const std::string &name,
                       const std::string &location, int capacity, std::time_t created_at)
    : venue_id_(venue_id), owner_id_(owner_id), name_(name), location_(location),
      capacity_(capacity), created_at_(created_at) {}

std::string indiepub::Venue::venue_id() const
{
    return venue_id_;
}

std::string indiepub::Venue::owner_id() const
{
    return owner_id_;
}

std::string indiepub::Venue::name() const
{
    return name_;
}

std::string indiepub::Venue::location() const
{
    return location_;
}

int indiepub::Venue::capacity() const
{
    return capacity_;
}

std::time_t indiepub::Venue::created_at() const
{
    return created_at_;
}

std::string indiepub::Venue::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("venue_id", venue_id_);
    json->put("owner_id", owner_id_);
    json->put("name", name_);
    json->put("location", location_);
    json->put("capacity", capacity_);
    json->put("created_at", timestamp_to_string(created_at_));
    return json->str();
}

indiepub::Venue indiepub::Venue::from_json(const std::string &json)
{
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

indiepub::Venue indiepub::Venue::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return Venue();
    }
}