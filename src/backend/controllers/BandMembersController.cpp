#include <backend/controllers/BandMembersController.hpp>
#include <backend/models/BandMember.hpp>
#include <backend/IndieBackModels.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

indiepub::BandMembersController::BandMembersController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace) : CassandraConnection(contact_points, username, password, keyspace)
{
    userController = std::make_shared<UsersController>(contact_points, username, password, keyspace);
    bandController = std::make_shared<BandsController>(contact_points, username, password, keyspace);
}

bool indiepub::BandMembersController::insertBandMember(const indiepub::BandMember &band_member)
{
    bool isValid = false;
    // Check if the band_id and user_id are valid UUIDs
    if (band_member.band_id().empty())
    {
        std::cerr << "Band ID cannot be empty" << std::endl;
        return isValid;
    }
    if (band_member.user_id().empty())
    {
        std::cerr << "User ID cannot be empty" << std::endl;
        return isValid;
    }

    if (!isConnected())
    {
        std::cerr << "Not connected to Cassandra" << std::endl;
        return isValid;
    }

    User _user = userController->getUserById(band_member.user_id()); // Check if user exists
    if (_user.user_id() != band_member.user_id())
    {
        std::cerr << "User with this ID does not exist" << std::endl;
        return isValid;
    }
    Band _band = bandController->getBandById(band_member.band_id()); // Check if band exists
    if (_band.band_id() != band_member.band_id())
    {
        std::cerr << "Band with this ID does not exist" << std::endl;
        return isValid;
    }

    std::string query = "INSERT INTO " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY +
                        " (band_id, user_id) VALUES (?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    CassUuid band_id;
    CassUuid user_id;
    if (cass_uuid_from_string(band_member.band_id().c_str(), &band_id) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + band_member.band_id());
    }
    if (cass_uuid_from_string(band_member.user_id().c_str(), &user_id) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + band_member.user_id());
    }
    cass_statement_bind_uuid(statement, 0, band_id);
    cass_statement_bind_uuid(statement, 1, user_id);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        std::cout << "Band member inserted successfully." << std::endl;
        isValid = true;
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        std::cerr << "Query execution failed: " << std::string(message, message_length) << std::endl;
        std::string error_message = "Query execution failed: " + std::string(message, message_length);
        std::cerr << error_message << std::endl;
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return isValid;
}

std::vector<indiepub::BandMember> indiepub::BandMembersController::getAllBandMembers()
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::BandMember> band_members;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);

        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            band_members.push_back(indiepub::BandMember::from_row(row));
        }

        cass_iterator_free(iterator);
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
    // If we reach here, it means no band members were found
    return band_members; // Return an empty vector in case of failure
}

indiepub::BandMember indiepub::BandMembersController::getBandMemberById(const std::string &band_id, const std::string &user_id)
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY + " WHERE band_id = ? AND user_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    CassUuid band_uuid;
    CassUuid user_uuid;
    if (cass_uuid_from_string(band_id.c_str(), &band_uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + band_id);
    }
    if (cass_uuid_from_string(user_id.c_str(), &user_uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + user_id);
    }
    cass_statement_bind_uuid(statement, 0, band_uuid);
    cass_statement_bind_uuid(statement, 1, user_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    indiepub::BandMember band_member;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_iterator_get_row(cass_iterator_from_result(result));
            band_member = indiepub::BandMember::from_row(row);
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
    // If we reach here, it means the band member was not found
    return band_member; // Return an empty BandMember object in case of failure
}

std::vector<indiepub::BandMember> indiepub::BandMembersController::getBandMembersByBandId(const std::string &band_id)
{
    std::string query = "SELECT * FROM " + this->keyspace_ + "." + BandMember::COLUMN_FAMILY + " WHERE band_id = ?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    CassUuid band_uuid;
    if (cass_uuid_from_string(band_id.c_str(), &band_uuid) != CASS_OK)
    {
        throw std::runtime_error("Invalid UUID string: " + band_id);
    }
    cass_statement_bind_uuid(statement, 0, band_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::BandMember> band_members;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);

        CassIterator *iterator = cass_iterator_from_result(result);

        while (cass_iterator_next(iterator))
        {
            const CassRow *row = cass_iterator_get_row(iterator);
            band_members.push_back(indiepub::BandMember::from_row(row));
        }

        cass_iterator_free(iterator);
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
    return band_members; // Return an empty vector in case of failure
}
