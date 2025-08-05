#ifndef INDIEPUB_ENDPOINTS_HPP
#define INDIEPUB_ENDPOINTS_HPP

#include <http/Method.hpp>
#include <http/Path.hpp>
#include <http/Response.hpp>
#include <http/Request.hpp>
#include <crypto/AuthCrypto.hpp>
#include <backend/controllers/CredentialsController.hpp>
#include <backend/controllers/UsersController.hpp>
#include <backend/controllers/EventController.hpp>
#include <backend/controllers/VenuesController.hpp>
#include <backend/controllers/VenueMembersController.hpp>
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

    static indiepub::EventController getEventController();

    static indiepub::VenuesController getVenuesController();

    static indiepub::VenueMembersController getVenueMembersController();

    static bool validateTokenAndId(const HttpRequest &request, HttpResponse &response, Path *path, indiepub::Credentials &creds, indiepub::User &user);

    static std::string decryptMessage(const std::string &value);

public:
    Endpoints(/* args */);

    ~Endpoints();

    static void signInHandler(const HttpRequest &request, HttpResponse &response, Path *path);

    static std::string tokenGenerator(std::string &pwHash);

    static void signUpHandler(const HttpRequest &request, HttpResponse &response, Path *path);

    static std::string hashing(std::string &value);

    static void validateHeaders(const HttpRequest &request, HttpResponse &response, Path *path);

    static void fetchEventsHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void createEventHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchPostsHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void createPostHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchUserInfoHandler(const HttpRequest &request, HttpResponse &response, Path* path);
    
    static void updateUserInfoHandler(const HttpRequest &request, HttpResponse &response, Path *path);

    static void addVenueProfileHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchVenueProfileHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void addBandProfileHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchBandProfileHandler(const HttpRequest &request, HttpResponse &response, Path* path);
};

#endif // INDIEPUB_ENDPOINTS_HPP