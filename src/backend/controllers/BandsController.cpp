#include <backend/controllers/BandsController.hpp>
#include <backend/models/Band.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::BandsController::BandsController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace)
{
}

bool indiepub::BandsController::insertBand(const indiepub::Band &band)
{
    bool isValid = false;
    // Check if the band_id is a valid UUID
    if (band.band_id().empty())
    {
        std::cerr << "Band ID cannot be empty" << std::endl;
        return isValid;
    }
    if (band.name().empty())
    {
        std::cerr << "Name cannot be empty" << std::endl;
        return isValid;
    }
    if (band.genre().empty())
    {
        std::cerr << "Genre cannot be empty" << std::endl;
        return isValid;
    }
    if (band.description().empty())
    {
        std::cerr << "Description cannot be empty" << std::endl;
        return isValid;
    }
    if (band.created_at() <= 0)
    {
        std::cerr << "Created at timestamp must be positive" << std::endl;
        return isValid;
    }
    // Check if the band already exists
    indiepub::Band existingBand = getBandById(band.band_id());
    if (existingBand.band_id() == band.band_id())
    {
        std::cerr << "Band with this ID already exists" << std::endl;
        return isValid;
    }
    // Check if the band already exists by name and genre
    indiepub::Band existingBandByNameAndGenre = getBandBy(band.name(), band.genre());
    if (existingBandByNameAndGenre.name() == band.name())
    {
        std::cerr << "Band with this name and genre already exists" << std::endl;
        return isValid;
    }
    if (!isConnected())
    {
        std::cerr << "Not connected to Cassandra" << std::endl;
        return isValid;
    }

    std::string query = "INSERT INTO " + keyspace_ + ".bands (band_id, name, genre, description, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid band_id;
    if (cass_uuid_from_string(band.band_id().c_str(), &band_id) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + band.band_id() << std::endl;
        return isValid;
    }
    cass_statement_bind_uuid(statement, 0, band_id);
    cass_statement_bind_string(statement, 1, band.name().c_str());
    cass_statement_bind_string(statement, 2, band.genre().c_str());
    cass_statement_bind_string(statement, 3, band.description().c_str());
    cass_statement_bind_int64(statement, 4, band.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK)
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        std::cerr << error_message << std::endl;
    }
    else
    {
        std::cout << "Query executed successfully." << std::endl;
        isValid = true;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::Band> indiepub::BandsController::getAllBands()
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Band::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::Band> bands;

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator))
            {
                const CassRow *row = cass_iterator_get_row(iterator);
                bands.push_back(indiepub::Band::from_row(row));
            }
            cass_iterator_free(iterator);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return bands;
}

indiepub::Band indiepub::BandsController::getBandById(const std::string &band_id)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Band::COLUMN_FAMILY + " WHERE band_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    indiepub::Band band;
    CassUuid uuid;
    if (cass_uuid_from_string(band_id.c_str(), &uuid) != CASS_OK)
    {
        std::cerr << "Invalid UUID string: " + band_id << std::endl;
        return band;
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            band = indiepub::Band::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return band;
}

indiepub::Band indiepub::BandsController::getBandByName(const std::string &name)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Band::COLUMN_FAMILY + " WHERE name = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    cass_statement_bind_string(statement, 0, name.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::Band band;

    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            band = indiepub::Band::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return band;
}

indiepub::Band indiepub::BandsController::getBandBy(const std::string &name, const std::string &genre)
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::Band::COLUMN_FAMILY + " WHERE name = ? AND genre = ? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, genre.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::Band band;
    
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            band = indiepub::Band::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return band;
}
