#ifndef INDIEPUB_MODELS_HPP
#define INDIEPUB_MODELS_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub {

    // Helper function to convert timestamp to ISO 8601 string
    std::string timestamp_to_string(std::time_t time);

    // Helper function to parse ISO 8601 string to timestamp
    std::time_t string_to_timestamp(const std::string& str);
    

} // namespace indiepub

#endif // INDIEPUB_MODELS_HPP