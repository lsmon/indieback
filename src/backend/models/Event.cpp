#include <backend/models/Event.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

indiepub::Event::Event(const std::string &event_id, const std::string &venue_id,
                       const std::string &band_id, const std::string &creator_id,
                       const std::string &name, std::time_t date, double price,
                       int capacity, int sold)
    : event_id_(event_id), venue_id_(venue_id), band_id_(band_id),
      creator_id_(creator_id), name_(name), date_(date),
      price_(price), capacity_(capacity), sold_(sold) {}

std::string indiepub::Event::event_id() const
{
    return event_id_;
}

std::string indiepub::Event::venue_id() const
{
    return venue_id_;
}

std::string indiepub::Event::band_id() const
{
    return band_id_;
}

std::string indiepub::Event::creator_id() const
{
    return creator_id_;
}

std::string indiepub::Event::name() const
{
    return name_;
}

std::time_t indiepub::Event::date() const
{
    return date_;
}

double indiepub::Event::price() const
{
    return price_;
}

int indiepub::Event::capacity() const
{
    return capacity_;
}

int indiepub::Event::sold() const
{
    return sold_;
}

std::string indiepub::Event::to_json() const
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

indiepub::Event indiepub::Event::from_json(const std::string &json)
{
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

indiepub::Event indiepub::Event::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return Event();
    }
}