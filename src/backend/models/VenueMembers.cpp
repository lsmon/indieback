#include <backend/models/VenueMembers.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <util/logging/Log.hpp>
#include <util/String.hpp>
#include <string>
#include <memory>

const std::string indiepub::VenueMembers::COLUMN_FAMILY = "venue_members";
const std::string indiepub::VenueMembers::PK_VENUE_ID = "venue_id";
const std::string indiepub::VenueMembers::CK_VENUE_USER_ID = "user_id";
const std::string indiepub::VenueMembers::CK_JOINED_AT = "joined_at";
const std::string indiepub::VenueMembers::IDX_ROLE = "role";
const std::string indiepub::VenueMembers::IDX_ACTIVE = "active";

indiepub::VenueMembers::VenueMembers(const std::string &venue_id, const std::string &user_id, const std::string &role,
                                     std::time_t joined_at, bool is_active)
    : venue_id_(venue_id), member_id_(user_id), role_(role), joined_at_(joined_at), is_active_(is_active) {}

std::string indiepub::VenueMembers::venue_id() const
{
    return venue_id_;
}

std::string indiepub::VenueMembers::user_id() const
{
    return member_id_;
}

std::string indiepub::VenueMembers::role() const
{
    return role_;
}

std::time_t indiepub::VenueMembers::joined_at() const
{
    return joined_at_;
}

bool indiepub::VenueMembers::is_active() const
{
    return is_active_;
}

void indiepub::VenueMembers::venue_id(const std::string &venue_id)
{
    venue_id_ = venue_id;
}

void indiepub::VenueMembers::user_id(const std::string &user_id)
{
    member_id_ = user_id;
}

void indiepub::VenueMembers::role(const std::string &role)
{
    role_ = role;
}

void indiepub::VenueMembers::joined_at(const std::time_t &joined_at)
{
    joined_at_ = joined_at;
}

void indiepub::VenueMembers::is_active(bool active)
{
    is_active_ = active;
}

std::string indiepub::VenueMembers::to_json() const
{
    try 
    {
        std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
        json->put("venue_id", venue_id_);
        json->put("user_id", member_id_);
        json->put("role", role_);
        json->put("joined_at", indiepub::timestamp_to_string(joined_at_));
        json->put("is_active", is_active_);
        return json->str();
    }
    catch (std::exception &e)
    {
        throw std::runtime_error(e.what());
    }
}

indiepub::VenueMembers indiepub::VenueMembers::from_json(const std::string &json)
{
    try
    {
        VenueMembers member;
        std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
        member.venue_id_ = jsonObject->get("venue_id").str();
        member.member_id_ = jsonObject->get("user_id").str();
        member.role_ = jsonObject->get("role").str();
        member.joined_at_ = indiepub::string_to_timestamp(jsonObject->get("joined_at").str());
        member.is_active_ = jsonObject->get("is_active").str() == "true";
        return member;
    }
    catch (std::runtime_error &ex)
    {
        throw std::runtime_error(ex.what());
    }
}

indiepub::VenueMembers indiepub::VenueMembers::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
            throw std::runtime_error("Row is null");
        }

        // Extract venue_id (UUID)
        CassUuid uuid;
        const CassValue *venue_id_value = cass_row_get_column_by_name(row, PK_VENUE_ID.c_str());
        if (cass_value_get_uuid(venue_id_value, &uuid) != CASS_OK)
        {
            throw std::runtime_error("Failed to get venue_id");
        }
        char venue_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(uuid, venue_id_str);
        std::string venue_id(venue_id_str);
        // Extract user_id (UUID)
        
        const CassValue *user_id_value = cass_row_get_column_by_name(row, CK_VENUE_USER_ID.c_str());
        if (cass_value_get_uuid(user_id_value, &uuid) != CASS_OK)
        {
            throw std::runtime_error("Failed to get user_id");
        }
        char user_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(uuid, user_id_str);
        std::string user_id(user_id_str);
        // Extract role (TEXT)P
        const char *role;
        size_t role_length = 0;
        const CassValue *role_value = cass_row_get_column_by_name(row, IDX_ROLE.c_str());
        if (cass_value_get_string(role_value, &role, &role_length) != CASS_OK)
        {
            LOG_WARN << "Failed to get role";
            role = "";
        }
        // Extract joined_at (TIMESTAMP)
        cass_int64_t joined_at;
        const CassValue *joined_at_value = cass_row_get_column_by_name(row, CK_JOINED_AT.c_str());
        if (cass_value_get_int64(joined_at_value, &joined_at) != CASS_OK)
        {
            throw std::runtime_error("Failed to get joined_at");
        }
        // Extract is_active (BOOLEAN)
        cass_bool_t cass_is_active = cass_false;
        const CassValue *is_active_value = cass_row_get_column_by_name(row, IDX_ACTIVE.c_str());
        if (cass_value_get_bool(is_active_value, &cass_is_active) != CASS_OK)
        {
            throw std::runtime_error("Failed to get is_active");
        }
        bool is_active = cass_is_active == cass_true;
        // Construct and return the VenueMembers object
        indiepub::VenueMembers member(
            venue_id,
            user_id,
            std::string(role, role_length),
            static_cast<std::time_t>(joined_at),
            is_active
        );
        return member;
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << "Error: " << e.what();
        throw std::runtime_error("Failed to construct VenueMembers from row: " + std::string(e.what()));
    }
}