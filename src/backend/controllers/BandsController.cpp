#include <backend/controllers/BandsController.hpp>
#include <backend/models/Band.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::BandsController::BandsController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::BandsController::insertBand(const indiepub::Band &band) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + keyspace_ + ".bands (band_id, name, genre, description, created_at) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    CassUuid band_id;
    if (cass_uuid_from_string(band.band_id().c_str(), &band_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band.band_id());
    }
    cass_statement_bind_uuid(statement, 0, band_id);
    cass_statement_bind_string(statement, 1, band.name().c_str());
    cass_statement_bind_string(statement, 2, band.genre().c_str());
    cass_statement_bind_string(statement, 3, band.description().c_str());
    cass_statement_bind_int64(statement, 4, band.created_at());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);

    if (cass_future_error_code(query_future) != CASS_OK) {
        const char* message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        throw std::runtime_error(error_message);
    } else {
        std::cout << "Query executed successfully." << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
}

std::vector<indiepub::Band> indiepub::BandsController::getAllBands() {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + ".bands";
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::Band> bands;
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator)) {
                const CassRow *row = cass_iterator_get_row(iterator);
                bands.push_back(indiepub::Band::from_row(row));
            }
            cass_iterator_free(iterator);
        }
        cass_result_free(result);
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return bands;
}

indiepub::Band indiepub::BandsController::getBandById(const std::string &band_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + ".bands WHERE band_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid uuid;
    if (cass_uuid_from_string(band_id.c_str(), &uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band_id);
    }
    cass_statement_bind_uuid(statement, 0, uuid);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::Band::from_row(row);
        } else {
            throw std::runtime_error("Band not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    // If we reach here, it means the band was not found
    throw std::runtime_error("Band not found");
}

indiepub::Band indiepub::BandsController::getBandBy(const std::string &name, const std::string &genre) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + keyspace_ + ".bands WHERE name = ? AND genre = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    cass_statement_bind_string(statement, 0, name.c_str());
    cass_statement_bind_string(statement, 1, genre.c_str());
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    const CassResult *result = nullptr;
    if (cass_future_error_code(query_future) == CASS_OK) {
        result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            return indiepub::Band::from_row(row);
        } else {
            throw std::runtime_error("Band not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    if (query_future != nullptr) {
        cass_future_free(query_future);
    }
    if (result != nullptr) { 
        cass_result_free(result);
        cass_iterator_free(cass_iterator_from_result(result));
    }
    // If we reach here, it means the band was not found
    throw std::runtime_error("Band not found");
}