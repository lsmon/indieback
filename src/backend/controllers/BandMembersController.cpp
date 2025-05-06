#include <backend/controllers/BandMembersController.hpp>
#include <backend/models/BandMember.hpp>
#include <backend/IndieBackModels.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

indiepub::BandMembersController::BandMembersController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace) {
}

void indiepub::BandMembersController::insertBandMember(const indiepub::BandMember &band_member) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY +
                        " (band_id, user_id) VALUES (?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    CassUuid band_id;
    CassUuid user_id;
    if (cass_uuid_from_string(band_member.band_id().c_str(), &band_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band_member.band_id());
    }
    if (cass_uuid_from_string(band_member.user_id().c_str(), &user_id) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band_member.user_id());
    }
    cass_statement_bind_uuid(statement, 0, band_id);
    cass_statement_bind_uuid(statement, 1, user_id);

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

std::vector<indiepub::BandMember> indiepub::BandMembersController::getAllBandMembers() {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::BandMember> band_members;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            band_members.emplace_back(indiepub::BandMember::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return band_members;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means no band members were found
    throw std::runtime_error("No band members found");
}

indiepub::BandMember indiepub::BandMembersController::getBandMemberById(const std::string &band_id, const std::string &user_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY + " WHERE band_id = ? AND user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    CassUuid band_uuid;
    CassUuid user_uuid;
    if (cass_uuid_from_string(band_id.c_str(), &band_uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band_id);
    }
    if (cass_uuid_from_string(user_id.c_str(), &user_uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, band_uuid);
    cass_statement_bind_uuid(statement, 1, user_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0) {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            indiepub::BandMember band_member = indiepub::BandMember::from_row(row);
            cass_result_free(result);
            return band_member;
        } else {
            throw std::runtime_error("Band member not found");
        }
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means the band member was not found
    throw std::runtime_error("Band member not found");
}

std::vector<indiepub::BandMember> indiepub::BandMembersController::getBandMembersByBandId(const std::string &band_id) {
    if (!isConnected()) {
        throw std::runtime_error("Not connected to Cassandra");
    }

    std::string query = "SELECT * FROM " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY + " WHERE band_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid band_uuid;
    if (cass_uuid_from_string(band_id.c_str(), &band_uuid) != CASS_OK) {
        throw std::runtime_error("Invalid UUID string: " + band_id);
    }
    cass_statement_bind_uuid(statement, 0, band_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK) {
        const CassResult *result = cass_future_get_result(query_future);
        std::vector<indiepub::BandMember> band_members;
        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator)) {
            const CassRow *row = cass_iterator_get_row(iterator);
            band_members.emplace_back(indiepub::BandMember::from_row(row));
        }

        cass_iterator_free(iterator);
        cass_result_free(result);
        return band_members;
    } else {
        throw std::runtime_error("Failed to execute query");
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    // If we reach here, it means no band members were found
    throw std::runtime_error("No band members found for the given band ID");
}
