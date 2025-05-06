#include <backend/models/Band.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::Band::COLUMN_FAMILY = "bands";
const std::string indiepub::Band::IDX_BANDS_NAME = "name";
const std::string indiepub::Band::IDX_BANDS_GENRE = "genre";

indiepub::Band::Band(const std::string &band_id, const std::string &name,
                     const std::string &genre, const std::string &description, std::time_t created_at)
    : band_id_(band_id), name_(name), genre_(genre),
      description_(description), created_at_(created_at) {}

std::string indiepub::Band::band_id() const
{
    return band_id_;
}

std::string indiepub::Band::name() const
{
    return name_;
}

std::string indiepub::Band::genre() const
{
    return genre_;
}

std::string indiepub::Band::description() const
{
    return description_;
}

std::time_t indiepub::Band::created_at() const
{
    return created_at_;
}

std::string indiepub::Band::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("band_id", band_id_);
    json->put("name", name_);
    json->put("genre", genre_);
    json->put("description", description_);
    json->put("created_at", timestamp_to_string(created_at_));
    return json->str();
}

indiepub::Band indiepub::Band::from_json(const std::string &json)
{
    Band band;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    band.band_id_ = jsonObject->get("band_id").str();
    band.name_ = jsonObject->get("name").str();
    band.genre_ = jsonObject->get("genre").str();
    band.description_ = jsonObject->get("description").str();
    band.created_at_ = string_to_timestamp(jsonObject->get("created_at").str());
    return band;
}

indiepub::Band indiepub::Band::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
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
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return Band();
    }
}