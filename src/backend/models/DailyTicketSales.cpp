
#include <backend/models/DailyTicketSales.hpp>
#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

const std::string indiepub::DailyTicketSales::COLUMN_FAMILY = "daily_ticket_sales";

indiepub::DailyTicketSales::DailyTicketSales(const std::string& event_id, const std::string& sale_date, int tickets_sold)
    : event_id_(event_id), sale_date_(sale_date), tickets_sold_(tickets_sold) {}

std::string indiepub::DailyTicketSales::event_id() const {
    return event_id_;
}

std::string indiepub::DailyTicketSales::sale_date() const {
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
    sales.sale_date_ = jsonObject->get("sale_date").str();
    sales.tickets_sold_ = std::stoi(jsonObject->get("tickets_sold").str());
    return sales;
}

indiepub::DailyTicketSales indiepub::DailyTicketSales::from_row(const CassRow *row) {
    try {
        if (row == nullptr) {
            throw std::runtime_error("Row is null");
        }
        const char *event_id;
        size_t event_id_length;
        cass_value_get_string(cass_row_get_column(row, 0), &event_id, &event_id_length);
    
        const char *sale_date;
        size_t sale_date_length;
        cass_value_get_string(cass_row_get_column(row, 1), &sale_date, &sale_date_length);
    
        int tickets_sold;
        cass_value_get_int32(cass_row_get_column(row, 2), &tickets_sold);
    
        return DailyTicketSales(std::string(event_id, event_id_length),
                                std::string(sale_date, sale_date_length),
                                tickets_sold);
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return DailyTicketSales();
    }
}