#include <backend/models/TicketByUser.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::TicketByUser::COLUMN_FAMILY = "tickets_by_user";
const std::string indiepub::TicketByUser::IDX_TICKETS_EVENT_ID = "event_id";
const std::string indiepub::TicketByUser::IDX_TICKETS_PURCHASE_DATE = "purchase_date";

indiepub::TicketByUser::TicketByUser(const std::string &ticket_id, const std::string &user_id,
                                     const std::string &event_id, std::time_t purchase_date)
    : ticket_id_(ticket_id), user_id_(user_id), event_id_(event_id),
      purchase_date_(purchase_date) {}

std::string indiepub::TicketByUser::ticket_id() const
{
    return ticket_id_;
}

std::string indiepub::TicketByUser::user_id() const
{
    return user_id_;
}

std::string indiepub::TicketByUser::event_id() const
{
    return event_id_;
}

std::time_t indiepub::TicketByUser::purchase_date() const
{
    return purchase_date_;
}

std::string indiepub::TicketByUser::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("ticket_id", ticket_id_);
    json->put("user_id", user_id_);
    json->put("event_id", event_id_);
    json->put("purchase_date", timestamp_to_string(purchase_date_));
    return json->str();
}

indiepub::TicketByUser indiepub::TicketByUser::from_json(const std::string &json)
{
    TicketByUser ticket;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    ticket.ticket_id_ = jsonObject->get("ticket_id").str();
    ticket.user_id_ = jsonObject->get("user_id").str();
    ticket.event_id_ = jsonObject->get("event_id").str();
    ticket.purchase_date_ = string_to_timestamp(jsonObject->get("purchase_date").str());
    return ticket;
}

indiepub::TicketByUser indiepub::TicketByUser::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
            throw std::runtime_error("Row is null");
        }

        CassUuid ticket_id;
        const CassValue *ticket_id_value = cass_row_get_column_by_name(row, "ticket_id");
        if (cass_value_get_uuid(ticket_id_value, &ticket_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get ticket_id from row");
        }
        char ticket_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(ticket_id, ticket_id_str);

        CassUuid user_id;
        const CassValue *user_id_value = cass_row_get_column_by_name(row, "user_id");
        if (cass_value_get_uuid(user_id_value, &user_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get user_id from row");
        }
        char user_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(user_id, user_id_str);

        CassUuid event_id;
        const CassValue *event_id_value = cass_row_get_column_by_name(row, "event_id");
        if (cass_value_get_uuid(event_id_value, &event_id) != CASS_OK)
        {
            throw std::runtime_error("Failed to get event_id from row");
        }
        char event_id_str[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(event_id, event_id_str);

        cass_int64_t purchase_date;
        const CassValue *purchase_date_value = cass_row_get_column_by_name(row, "purchase_date");
        if (cass_value_get_int64(purchase_date_value, &purchase_date) != CASS_OK)
        {
            throw std::runtime_error("Failed to get purchase_date from row");
        }
        
        return TicketByUser(std::string(ticket_id_str, CASS_UUID_STRING_LENGTH - 1),
                            std::string(user_id_str, CASS_UUID_STRING_LENGTH - 1),
                            std::string(event_id_str, CASS_UUID_STRING_LENGTH - 1),
                            static_cast<std::time_t>(purchase_date));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    return TicketByUser();
}
