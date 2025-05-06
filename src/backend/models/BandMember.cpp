#include <backend/models/BandMember.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::BandMember::COLUMN_FAMILY = "band_members";

indiepub::BandMember::BandMember(const std::string &band_id, const std::string &user_id)
    : band_id_(band_id), user_id_(user_id) {}

std::string indiepub::BandMember::band_id() const
{
    return band_id_;
}

std::string indiepub::BandMember::user_id() const
{
    return user_id_;
}

std::string indiepub::BandMember::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("band_id", band_id_);
    json->put("user_id", user_id_);
    return json->str();
}

indiepub::BandMember indiepub::BandMember::from_json(const std::string &json)
{
    BandMember member;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    member.band_id_ = jsonObject->get("band_id").str();
    member.user_id_ = jsonObject->get("user_id").str();
    return member;
}

indiepub::BandMember indiepub::BandMember::from_row(const CassRow *row)
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

        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &user_id, &user_id_length);

        return BandMember(std::string(band_id, band_id_length),
                          std::string(user_id, user_id_length));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return BandMember();
    }
}