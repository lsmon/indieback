#include <backend/models/Venue.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>


const std::string indiepub::Venue::COLUMN_FAMILY = "venues";
const std::string indiepub::Venue::IDX_VENUES_NAME = "name";
const std::string indiepub::Venue::IDX_OWNERS_ID = "owner_id";
const std::string indiepub::Venue::IDX_VENUES_LOCATION = "location";

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
        // Extract venue_id (UUID)
        CassUuid venue_id;
        const CassValue *venue_id_value = cass_row_get_column_by_name(row, "venue_id");
        if (cass_value_get_uuid(venue_id_value, &venue_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get venue_id from row");
        }
        char venue_id_str[37];
        cass_uuid_string(venue_id, venue_id_str);

        // Extract owner_id (UUID)
        CassUuid owner_id;
        const CassValue *owner_id_value = cass_row_get_column_by_name(row, "owner_id");
        if (cass_value_get_uuid(owner_id_value, &owner_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get owner_id from row");
        }
        char owner_id_str[37];
        cass_uuid_string(owner_id, owner_id_str);

        // Extract name
        const CassValue *name_value = cass_row_get_column_by_name(row, "name");
        const char *name_str;
        size_t name_length;
        if (cass_value_get_string(name_value, &name_str, &name_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get name from row");
        }

        // Extract location
        const CassValue *location_value = cass_row_get_column_by_name(row, "location");
        const char *location_str;
        size_t location_length;
        if (cass_value_get_string(location_value, &location_str, &location_length) != CASS_OK)
        {
            throw std::runtime_error("Failed to get location from row");
        }

        // Extract capacity
        const CassValue *capacity_value = cass_row_get_column_by_name(row, "capacity");
        int32_t capacity;
        if (cass_value_get_int32(capacity_value, &capacity) != CASS_OK)
        {
            throw std::runtime_error("Failed to get capacity from row");
        }

        // Extract created_at
        const CassValue *created_at_value = cass_row_get_column_by_name(row, "created_at");
        cass_int64_t created_at;
        if (cass_value_get_int64(created_at_value, &created_at) != CASS_OK)
        {
            throw std::runtime_error("Failed to get created_at from row");
        }
        // Create and return the Venue object
        return Venue(
            std::string(venue_id_str), 
            std::string(owner_id_str), 
            std::string(name_str, name_length),
            std::string(location_str, location_length),
            capacity,
            static_cast<std::time_t>(created_at / 1000)
        ); // Convert from milliseconds to seconds
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return Venue();
    }
}