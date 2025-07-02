#ifndef INDIEPUB_ENDPOINTS_HPP
#define INDIEPUB_ENDPOINTS_HPP

#include <http/Method.hpp>
#include <http/Path.hpp>
#include <http/Response.hpp>
#include <http/Request.hpp>
#include <crypto/AuthCrypto.hpp>
#include <backend/controllers/CredentialsController.hpp>
#include <backend/controllers/UsersController.hpp>
#include <string>
#include <iostream>
#include <stdexcept>
#include <memory>

class Endpoints
{
private:

    static bool isValidEmail(const std::string &email);
    
    static bool isValidPassword(const std::string &password);

    static indiepub::CredentialsController getCredentialsController();
    static indiepub::UsersController getUsersController();

public:
    Endpoints(/* args */);

    ~Endpoints();

    static void signInHandler(const HttpRequest &request, HttpResponse &response, Path *path);

    static std::string tokenGenerator(std::string &pwHash);

    static void signUpHandler(const HttpRequest &request, HttpResponse &response, Path *path);

    static std::string hashing(std::string &value);

    static void fetchEventsHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void createEventHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchPostsHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void createPostHandler(const HttpRequest &request, HttpResponse &response, Path* path);


};

#endif // INDIEPUB_ENDPOINTS_HPP