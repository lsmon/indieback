#ifndef INDIEPUB_ENDPOINTS_HPP
#define INDIEPUB_ENDPOINTS_HPP

#include <http/Method.hpp>
#include <http/Path.hpp>
#include <http/Response.hpp>
#include <http/Request.hpp>
#include <string>
#include <iostream>
#include <stdexcept>

class Endpoints
{
private:
    static bool isValidEmail(const std::string &email);
    
    static bool isValidPassword(const std::string &password);

public:
    Endpoints(/* args */);
    ~Endpoints();

    static void signInHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void signUpHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchEventsHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void createEventHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void fetchPostsHandler(const HttpRequest &request, HttpResponse &response, Path* path);

    static void createPostHandler(const HttpRequest &request, HttpResponse &response, Path* path);


};

#endif // INDIEPUB_ENDPOINTS_HPP