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

        CassUuid band_id;
        const CassValue *band_id_value = cass_row_get_column_by_name(row, "band_id");
        if (cass_value_get_uuid(band_id_value, &band_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get band_id from row");
        }
        char band_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(band_id, band_id_str);

        CassUuid user_id;
        const CassValue *user_id_value = cass_row_get_column_by_name(row, "user_id");
        if (cass_value_get_uuid(user_id_value, &user_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get user_id from row");
        }
        char user_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(user_id, user_id_str);
        
        return BandMember(std::string(band_id_str, CASS_UUID_STRING_LENGTH - 1), std::string(user_id_str, CASS_UUID_STRING_LENGTH - 1));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return BandMember();
    }
}