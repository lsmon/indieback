#ifndef INDIEPUB_MODELS_HPP
#define INDIEPUB_MODELS_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>
#include <sstream>

namespace indiepub {

    // Helper function to convert timestamp to ISO 8601 string
    std::string timestamp_to_string(std::time_t time);

    // Helper function to parse ISO 8601 string to timestamp
    std::time_t string_to_timestamp(const std::string& str);
    

    enum UType {
        FAN,
        OWNER,
        ADMIN,
        EMPLOYEE,
        MEMBER,
        LEADER,
        MANAGER
    };

    struct VenueEmployeeType 
    {
        std::stringstream ss;
        VenueEmployeeType(UType u) 
        {
            switch (u) {
                case OWNER: ss << "owner"; break;
                case ADMIN: ss << "admin"; break;
                case EMPLOYEE: ss << "employee"; break;
                default: ss << "fan";
            }
        }
        operator std::string() const { return ss.str(); }
        UType type() const 
        {
            if (ss.str() == "owner") return OWNER;
            if (ss.str() == "admin") return ADMIN;
            if (ss.str() == "employee") return EMPLOYEE;
            return FAN; // Default case
        }
    };

    struct BandMemberType
    {
        std::stringstream ss;
        BandMemberType(UType u) 
        {
            switch (u) {
                case MEMBER: ss << "member"; break;
                case LEADER: ss << "leader"; break;
                case MANAGER: ss << "manager"; break;
                default: ss << "fan";
            }
        }
        operator std::string() const { return ss.str(); }
        UType type() const 
        {
            if (ss.str() == "member") return MEMBER;
            if (ss.str() == "leader") return LEADER;
            if (ss.str() == "manager") return MANAGER;
            return FAN; // Default case
        }
    };
} // namespace indiepub

#endif // INDIEPUB_MODELS_HPP