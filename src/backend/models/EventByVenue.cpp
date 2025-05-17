#include <backend/models/EventByVenue.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::EventByVenue::COLUMN_FAMILY = "events_by_venue";
const std::string indiepub::EventByVenue::IDX_EVENTS_EVENT_ID = "event_id";
const std::string indiepub::EventByVenue::IDX_EVENTS_BAND_ID = "band_id";
const std::string indiepub::EventByVenue::IDX_EVENTS_CAPACITY = "capacity";
const std::string indiepub::EventByVenue::IDX_EVENTS_CREATOR_ID = "creator_id";
const std::string indiepub::EventByVenue::IDX_EVENTS_NAME = "name";
const std::string indiepub::EventByVenue::IDX_EVENTS_PRICE = "price";
const std::string indiepub::EventByVenue::IDX_EVENTS_SOLD = "sold";

indiepub::EventByVenue::EventByVenue(const std::string &event_id, const std::string &venue_id,
                                     const std::string &band_id, const std::string &creator_id,
                                     const std::string &name, std::time_t date, double price,
                                     int capacity, int sold)
    : event_id_(event_id), venue_id_(venue_id), band_id_(band_id),
      creator_id_(creator_id), name_(name), date_(date),
      price_(price), capacity_(capacity), sold_(sold) {}

std::string indiepub::EventByVenue::event_id() const
{
    return event_id_;
}

std::string indiepub::EventByVenue::venue_id() const
{
    return venue_id_;
}

std::string indiepub::EventByVenue::band_id() const
{
    return band_id_;
}

std::string indiepub::EventByVenue::creator_id() const
{
    return creator_id_;
}

std::string indiepub::EventByVenue::name() const
{
    return name_;
}

std::time_t indiepub::EventByVenue::date() const
{
    return date_;
}

double indiepub::EventByVenue::price() const
{
    return price_;
}

int indiepub::EventByVenue::capacity() const
{
    return capacity_;
}

int indiepub::EventByVenue::sold() const
{
    return sold_;
}

std::string indiepub::EventByVenue::to_json() const
{
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

indiepub::EventByVenue indiepub::EventByVenue::from_json(const std::string &json)
{
    EventByVenue event;
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

indiepub::EventByVenue indiepub::EventByVenue::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
            throw std::runtime_error("Row is null");
        }

        CassUuid event_id;
        const CassValue *event_id_value = cass_row_get_column_by_name(row, "event_id");
        if (cass_value_get_uuid(event_id_value, &event_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get event_id from row");
        }
        char event_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(event_id, event_id_str);
        CassUuid venue_id;
        const CassValue *venue_id_value = cass_row_get_column_by_name(row, "venue_id");
        if (cass_value_get_uuid(venue_id_value, &venue_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get venue_id from row");
        }
        char venue_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(venue_id, venue_id_str);
        CassUuid band_id;
        const CassValue *band_id_value = cass_row_get_column_by_name(row, "band_id");
        if (cass_value_get_uuid(band_id_value, &band_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get band_id from row");
        }
        char band_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(band_id, band_id_str);
        CassUuid creator_id;
        const CassValue *creator_id_value = cass_row_get_column_by_name(row, "creator_id");
        if (cass_value_get_uuid(creator_id_value, &creator_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get creator_id from row");
        }
        char creator_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(creator_id, creator_id_str);
        const CassValue *name_value = cass_row_get_column_by_name(row, "name");
        const char *name;
        size_t name_length;
        cass_value_get_string(name_value, &name, &name_length);
        cass_int64_t date;
        const CassValue *date_value = cass_row_get_column_by_name(row, "date");
        if (cass_value_get_int64(date_value, &date) != CASS_OK)
        {
            throw std::runtime_error("Failed to get date from row");
        }
        cass_double_t price;
        const CassValue *price_value = cass_row_get_column_by_name(row, "price");
        if (cass_value_get_double(price_value, &price) != CASS_OK)
        {
            throw std::runtime_error("Failed to get price from row");
        }
        cass_int32_t capacity;
        const CassValue *capacity_value = cass_row_get_column_by_name(row, "capacity");
        if (cass_value_get_int32(capacity_value, &capacity) != CASS_OK)
        {
            throw std::runtime_error("Failed to get capacity from row");
        }
        cass_int32_t sold;
        const CassValue *sold_value = cass_row_get_column_by_name(row, "sold");
        if (cass_value_get_int32(sold_value, &sold) != CASS_OK)
        {
            throw std::runtime_error("Failed to get sold from row");
        }

        return EventByVenue(std::string(event_id_str, CASS_UUID_STRING_LENGTH - 1),
                            std::string(venue_id_str, CASS_UUID_STRING_LENGTH - 1),
                            std::string(band_id_str, CASS_UUID_STRING_LENGTH - 1),
                            std::string(creator_id_str, CASS_UUID_STRING_LENGTH - 1),
                            std::string(name, name_length),
                            static_cast<std::time_t>(date),
                            static_cast<double>(price),
                            static_cast<int>(capacity),
                            static_cast<int>(sold));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EventByVenue();
    }
}