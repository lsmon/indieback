#include <backend/models/TicketByEvent.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::TicketByEvent::COLUMN_FAMILY = "tickets_by_event";
const std::string indiepub::TicketByEvent::IDX_TICKETS_USER_ID = "user_id";
const std::string indiepub::TicketByEvent::IDX_TICKETS_PURCHASE_DATE = "purchase_date";

indiepub::TicketByEvent::TicketByEvent(const std::string &ticket_id, const std::string &user_id,
                         const std::string &event_id, std::time_t purchase_date)
    : ticket_id_(ticket_id), user_id_(user_id), event_id_(event_id),
      purchase_date_(purchase_date) {}

std::string indiepub::TicketByEvent::ticket_id() const
{
    return ticket_id_;
}

std::string indiepub::TicketByEvent::user_id() const
{
    return user_id_;
}

std::string indiepub::TicketByEvent::event_id() const
{
    return event_id_;
}

std::time_t indiepub::TicketByEvent::purchase_date() const
{
    return purchase_date_;
}

std::string indiepub::TicketByEvent::to_json() const
{
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("ticket_id", ticket_id_);
    json->put("user_id", user_id_);
    json->put("event_id", event_id_);
    json->put("purchase_date", timestamp_to_string(purchase_date_));
    return json->str();
}

indiepub::TicketByEvent indiepub::TicketByEvent::from_json(const std::string &json)
{
    TicketByEvent ticket;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    ticket.ticket_id_ = jsonObject->get("ticket_id").str();
    ticket.user_id_ = jsonObject->get("user_id").str();
    ticket.event_id_ = jsonObject->get("event_id").str();
    ticket.purchase_date_ = string_to_timestamp(jsonObject->get("purchase_date").str());
    return ticket;
}

indiepub::TicketByEvent indiepub::TicketByEvent::from_row(const CassRow *row)
{
    try
    {
        if (row == nullptr)
        {
            throw std::runtime_error("Row is null");
        }
        const char *ticket_id;
        size_t ticket_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &ticket_id, &ticket_id_length);

        const char *user_id;
        size_t user_id_length;
        cass_value_get_string(cass_row_get_column(row, 1), &user_id, &user_id_length);

        const char *event_id;
        size_t event_id_length;
        cass_value_get_string(cass_row_get_column(row, 2), &event_id, &event_id_length);

        cass_int64_t purchase_date;
        cass_value_get_int64(cass_row_get_column(row, 3), &purchase_date);

        return TicketByEvent(std::string(ticket_id, ticket_id_length),
                      std::string(user_id, user_id_length),
                      std::string(event_id, event_id_length),
                      purchase_date);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return TicketByEvent();
    }
}
