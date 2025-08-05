#include <backend/controllers/VenueMembersController.hpp>
#include <backend/models/VenueMembers.hpp>
#include <util/logging/Log.hpp>
#include <iostream>
#include <stdexcept>
#include <string>

indiepub::VenueMembersController::VenueMembersController(const std::string &contact_points, const std::string &username, const std::string &password, const std::string &keyspace)
    : CassandraConnection(contact_points, username, password, keyspace)
{
}

bool indiepub::VenueMembersController::insertVenueMember(const indiepub::VenueMembers &member)
{
    bool isValid = false;
    // Check if the venue_id and user_id are valid UUIDs
    if (member.venue_id().empty() || member.user_id().empty())
    {
        LOG_ERROR << "Venue ID and User ID cannot be empty";
        return isValid;
    }
    if (member.role().empty())
    {
        LOG_ERROR << "Role cannot be empty";
        return isValid;
    }
    if (member.joined_at() <= 0)
    {
        LOG_ERROR << "Joined at timestamp must be positive";
        return isValid;
    }

    if (!isConnected())
    {
        LOG_ERROR << "Not connected to Cassandra";
        return isValid;
    }

    // Check if the member already exists
    indiepub::VenueMembers existingMember = getVenueMemberById(member.venue_id(), member.user_id());
    if (existingMember.venue_id() == member.venue_id() && existingMember.user_id() == member.user_id())
    {
        LOG_ERROR << "Member with this venue ID and user ID already exists";
        return isValid;
    }

    std::string query = "INSERT INTO " + keyspace_ + "." + indiepub::VenueMembers::COLUMN_FAMILY + " (venue_id, user_id, role, joined_at, active) VALUES (?, ?, ?, ?, ?)";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    
    CassUuid venue_uuid, user_uuid;
    cass_uuid_from_string(member.venue_id().c_str(), &venue_uuid);
    cass_uuid_from_string(member.user_id().c_str(), &user_uuid);
    
    cass_statement_bind_uuid(statement, 0, venue_uuid);
    cass_statement_bind_uuid(statement, 1, user_uuid);
    cass_statement_bind_string(statement, 2, member.role().c_str());
    
    cass_statement_bind_int64(statement, 3, member.joined_at());
    
    cass_statement_bind_bool(statement, 4, static_cast<cass_bool_t>(member.is_active()));

    CassFuture *future = cass_session_execute(session, statement);
    
    if (cass_future_error_code(future) == CASS_OK)
    {
        isValid = true;
        LOG_INFO << "Inserted Venue Member: " << member.to_json();
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(future, &message, &message_length);
        LOG_ERROR << "Failed to insert Venue Member: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(future);
    return isValid;
}

bool indiepub::VenueMembersController::updateVenueMember(const indiepub::VenueMembers &member)
{
    bool isValid = false;
    // Check if the venue_id and user_id are valid UUIDs
    if (member.venue_id().empty() || member.user_id().empty())
    {
        LOG_ERROR << "Venue ID and User ID cannot be empty";
        return isValid;
    }
    if (member.role().empty())
    {
        LOG_ERROR << "Role cannot be empty";
        return isValid;
    }
    if (member.joined_at() <= 0)
    {
        LOG_ERROR << "Joined at timestamp must be positive";
        return isValid;
    }

    if (!isConnected())
    {
        LOG_ERROR << "Not connected to Cassandra";
        return isValid;
    }

    std::string query = "UPDATE " + keyspace_ + "." + indiepub::VenueMembers::COLUMN_FAMILY + " SET role=?, active=? WHERE venue_id=? AND user_id=? AND joined_at=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 5);
    
    CassUuid venue_uuid, user_uuid;
    cass_uuid_from_string(member.venue_id().c_str(), &venue_uuid);
    cass_uuid_from_string(member.user_id().c_str(), &user_uuid);
    
    cass_statement_bind_string(statement, 0, member.role().c_str());
    cass_statement_bind_bool(statement, 1, static_cast<cass_bool_t>(member.is_active()));
    
    cass_statement_bind_uuid(statement, 2, venue_uuid);
    cass_statement_bind_uuid(statement, 3, user_uuid);
    cass_statement_bind_int64(statement, 4, member.joined_at());

    CassFuture *future = cass_session_execute(session, statement);
    
    if (cass_future_error_code(future) == CASS_OK)
    {
        isValid = true;
        LOG_INFO << "Updated Venue Member: " << member.to_json();
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(future, &message, &message_length);
        LOG_ERROR << "Failed to update Venue Member: " << std::string(message, message_length);
    }
    
    cass_statement_free(statement);
    cass_future_free(future);
    
    return isValid;
}

std::vector<indiepub::VenueMembers> indiepub::VenueMembersController::getAllVenueMembers()
{
    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::VenueMembers::COLUMN_FAMILY;
    CassStatement *statement = cass_statement_new(query.c_str(), 0);
    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::VenueMembers> members;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator))
            {
                const CassRow *row = cass_iterator_get_row(iterator);
                members.push_back(indiepub::VenueMembers::from_row(row));
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
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return members;
}

indiepub::VenueMembers indiepub::VenueMembersController::getVenueMemberById(const std::string &venue_id, const std::string &user_id)
{
    if (venue_id.empty() || user_id.empty())
    {
        LOG_ERROR << "Venue ID and User ID cannot be empty";
        return indiepub::VenueMembers();
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::VenueMembers::COLUMN_FAMILY + " WHERE venue_id=? AND user_id=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 2);
    
    CassUuid venue_uuid, user_uuid;
    cass_uuid_from_string(venue_id.c_str(), &venue_uuid);
    cass_uuid_from_string(user_id.c_str(), &user_uuid);
    
    cass_statement_bind_uuid(statement, 0, venue_uuid);
    cass_statement_bind_uuid(statement, 1, user_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    
    indiepub::VenueMembers member;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            member = indiepub::VenueMembers::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    
    cass_statement_free(statement);
    cass_future_free(query_future);
    
    return member;
}

indiepub::VenueMembers indiepub::VenueMembersController::getVenueMemberByUserId(const std::string &user_id)
{
    if (user_id.empty())
    {
        LOG_ERROR << "User ID cannot be empty";
        return indiepub::VenueMembers();
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::VenueMembers::COLUMN_FAMILY + " WHERE user_id=? ALLOW FILTERING";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    
    CassUuid user_uuid;
    cass_uuid_from_string(user_id.c_str(), &user_uuid);
    
    cass_statement_bind_uuid(statement, 0, user_uuid);

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    
    indiepub::VenueMembers member;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            const CassRow *row = cass_result_first_row(result);
            member = indiepub::VenueMembers::from_row(row);
        }
        cass_result_free(result);
    }
    else
    {
        const char *message;
        size_t message_length;
        cass_future_error_message(query_future, &message, &message_length);
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    
    cass_statement_free(statement);
    cass_future_free(query_future);
    
    return member;
}


std::vector<indiepub::VenueMembers> indiepub::VenueMembersController::getVenueMembersByRole(const std::string &role)
{
    if (role.empty())
    {
        LOG_ERROR << "Role cannot be empty";
        return {};
    }

    std::string query = "SELECT * FROM " + keyspace_ + "." + indiepub::VenueMembers::COLUMN_FAMILY + " WHERE role=?";
    CassStatement *statement = cass_statement_new(query.c_str(), 1);
    
    cass_statement_bind_string(statement, 0, role.c_str());

    CassFuture *query_future = cass_session_execute(session, statement);
    cass_future_wait(query_future);
    std::vector<indiepub::VenueMembers> members;
    if (cass_future_error_code(query_future) == CASS_OK)
    {
        const CassResult *result = cass_future_get_result(query_future);
        if (cass_result_row_count(result) > 0)
        {
            CassIterator *iterator = cass_iterator_from_result(result);
            while (cass_iterator_next(iterator))
            {
                const CassRow *row = cass_iterator_get_row(iterator);
                members.push_back(indiepub::VenueMembers::from_row(row));
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
        LOG_ERROR << "Query execution failed: " << std::string(message, message_length);
    }
    cass_statement_free(statement);
    cass_future_free(query_future);
    return members;
}
