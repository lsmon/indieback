#ifndef INDIEPUB_BAND_HPP
#define INDIEPUB_BAND_HPP

#include <string>
#include <vector>
#include <optional>
#include <ctime>
#include <cassandra.h>

namespace indiepub
{
    class Band
    {
    public:
        Band() = default;
        Band(const std::string &band_id, const std::string &name, const std::string &genre,
             const std::string &description, std::time_t created_at);

        static const std::string COLUMN_FAMILY;
        static const std::string IDX_BANDS_NAME;
        static const std::string IDX_BANDS_GENRE;
        
        // Getters
        std::string band_id() const;
        std::string name() const;
        std::string genre() const;
        std::string description() const;
        std::time_t created_at() const;

        // JSON serialization
        std::string to_json() const;

        static Band from_json(const std::string &json);
        static Band from_row(const CassRow *row);

    private:
        std::string band_id_; // UUID
        std::string name_;
        std::string genre_;
        std::string description_;
        std::time_t created_at_;
    };
} // namespace indiepub

#endif // INDIEPUB_BAND_HPP