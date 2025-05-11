#ifndef INCLUDE_BACKEND_CONTROLLERS_POSTS_BY_DATE_CONTROLLER_HPP
#define INCLUDE_BACKEND_CONTROLLERS_POSTS_BY_DATE_CONTROLLER_HPP

#include <backend/CassandraConnection.hpp>
#include <backend/models/PostsByDate.hpp>
#include <backend/controllers/UsersController.hpp>
#include <memory>

namespace indiepub {

    class PostsByDateController : public CassandraConnection {
    public:
        PostsByDateController() = default;
        PostsByDateController(const std::string& contact_points, const std::string& username, const std::string& password, const std::string& keyspace);

        bool insertPost(const indiepub::PostsByDate& post);
        std::vector<indiepub::PostsByDate> getAllPosts();
        indiepub::PostsByDate getPostById(const std::string& post_id);
        std::vector<indiepub::PostsByDate> getPostsByUserId(const std::string& user_id);

    private:
        std::shared_ptr<UsersController> userController;
    };

} // namespace indiepub

#endif // INCLUDE_BACKEND_CONTROLLERS_POSTS_BY_DATE_CONTROLLER_HPP