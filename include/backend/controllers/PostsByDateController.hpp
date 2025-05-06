#ifndef INCLUDE_BACKEND_CONTROLLERS_POSTS_BY_DATE_CONTROLLER_HPP
#define INCLUDE_BACKEND_CONTROLLERS_POSTS_BY_DATE_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/PostsByDate.hpp>

namespace indiepub {

    class PostsByDateController : public CassandraConnection {
    public:
        PostsByDateController() = default;
        PostsByDateController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        void insertPost(const indiepub::PostsByDate& post);
        std::vector<indiepub::PostsByDate> getAllPosts();
        indiepub::PostsByDate getPostById(const std::string& post_id);
        std::vector<indiepub::PostsByDate> getPostsByUserId(const std::string& user_id);
    };

} // namespace indiepub

#endif // INCLUDE_BACKEND_CONTROLLERS_POSTS_BY_DATE_CONTROLLER_HPP