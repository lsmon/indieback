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

        CassUuid band_id;
        const CassValue *band_id_value = cass_row_get_column_by_name(row, "band_id");
        if (cass_value_get_uuid(band_id_value, &band_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get band_id from row");
        }
        char band_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(band_id, band_id_str);
        
        const CassValue *name_value = cass_row_get_column_by_name(row, "name");
        const char *name_str;
        size_t name_size;
        if (cass_value_get_string(name_value, &name_str, &name_size) != CASS_OK)
        {
            throw std::runtime_error("Failed to get name from row");
        }

        const CassValue *genre_value = cass_row_get_column_by_name(row, "genre");
        const char *genre_str;
        size_t genre_size;
        if (cass_value_get_string(genre_value, &genre_str, &genre_size) != CASS_OK)
        {
            throw std::runtime_error("Failed to get genre from row");
        }
        const CassValue *description_value = cass_row_get_column_by_name(row, "description");
        const char *description_str;
        size_t description_size;
        if (cass_value_get_string(description_value, &description_str, &description_size) != CASS_OK)
        {
            throw std::runtime_error("Failed to get description from row");
        }
        
        const CassValue *created_at_value = cass_row_get_column_by_name(row, "created_at");
        cass_int64_t created_at;
        if (cass_value_get_int64(created_at_value, &created_at) != CASS_OK)
        {
            throw std::runtime_error("Failed to get created_at from row");
        }
        
        return Band(
            band_id_str, 
            std::string(name_str, name_size), 
            std::string(genre_str, genre_size),
            std::string(description_str, description_size), 
            static_cast<std::time_t>(created_at));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return Band();
    }
}