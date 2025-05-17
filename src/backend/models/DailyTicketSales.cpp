
#include <backend/models/DailyTicketSales.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::DailyTicketSales::COLUMN_FAMILY = "daily_ticket_sales";

indiepub::DailyTicketSales::DailyTicketSales(const std::string& event_id, std::time_t sale_date, int tickets_sold)
    : event_id_(event_id), sale_date_(sale_date), tickets_sold_(tickets_sold) {}

std::string indiepub::DailyTicketSales::event_id() const {
    return event_id_;
}

std::time_t indiepub::DailyTicketSales::sale_date() const {
    return sale_date_;
}

int indiepub::DailyTicketSales::tickets_sold() const {
    return tickets_sold_;
}

std::string indiepub::DailyTicketSales::to_json() const {
    std::unique_ptr<JSONObject> json = std::make_unique<JSONObject>();
    json->put("event_id", event_id_);
    json->put("sale_date", sale_date_);
    json->put("tickets_sold", tickets_sold_);
    return json->str();
}

indiepub::DailyTicketSales indiepub::DailyTicketSales::from_json(const std::string& json) {
    DailyTicketSales sales;
    std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(json);
    sales.event_id_ = jsonObject->get("event_id").str();
    sales.sale_date_ = indiepub::string_to_timestamp(jsonObject->get("sale_date").str());
    sales.tickets_sold_ = std::stoi(jsonObject->get("tickets_sold").str());
    return sales;
}

indiepub::DailyTicketSales indiepub::DailyTicketSales::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }

        const CassValue *event_id_value = cass_row_get_column_by_name(row, "event_id");
        CassUuid event_id_value_uuid;
        if (cass_value_get_uuid(event_id_value, &event_id_value_uuid) != CASS_OK) {
            throw std::runtime_error("Failed to get event_id from row");
        }
        char event_id[CASS_UUID_STRING_LENGTH];
        cass_uuid_string(event_id_value_uuid, event_id);

        cass_int64_t sale_date;
        const CassValue *sale_date_value = cass_row_get_column_by_name(row, "sale_date");
        if (cass_value_get_int64(sale_date_value, &sale_date) != CASS_OK) {
            throw std::runtime_error("Failed to get sale_date from row");
        }

        cass_int32_t tickets_sold;
        const CassValue *tickets_sold_value = cass_row_get_column_by_name(row, "tickets_sold");
        // COUNTER columns in Cassandra are stored as 64-bit integers (cass_int64_t)
        cass_int64_t tickets_sold_counter;
        if (cass_value_get_int64(tickets_sold_value, &tickets_sold_counter) != CASS_OK) {
            throw std::runtime_error("Failed to get tickets_sold (counter) from row");
        }
        tickets_sold = static_cast<int>(tickets_sold_counter);

        return DailyTicketSales(
            std::string(event_id, CASS_UUID_STRING_LENGTH - 1),
            sale_date,
            tickets_sold
        );
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return DailyTicketSales();
    }
}