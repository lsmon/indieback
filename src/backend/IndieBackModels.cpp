#include <backend/IndieBackModels.hpp>
#include <JSON.hpp>
#include <string>
#include <memory>

std::string indiepub::timestamp_to_string(std::time_t time) {
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", std::gmtime(&time));
    return buffer;
}

std::time_t indiepub::string_to_timestamp(const std::string& str) {
    std::tm tm = {};
    strptime(str.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tm);
    return std::mktime(&tm);
}

