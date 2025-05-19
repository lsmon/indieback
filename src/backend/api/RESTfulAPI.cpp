#include <backend/api/RESTfulAPI.hpp>
#include <backend/api/Endpoints.hpp>
#include <util/logging/Log.hpp>

RESTfulAPI::RESTfulAPI()
{
    apiServer = std::make_unique<HttpServer>("localhost", "8080", 1024, 4);
}

void RESTfulAPI::initEndpointHandlers() {
    LOG_INFO << "Mapping endpoints";
    LOG_INFO << "/login POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/login", Endpoints::signInHandler);
    LOG_INFO << "/signup POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/signup", Endpoints::signUpHandler);
    LOG_INFO << "/events GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/events", Endpoints::fetchEventsHandler);
    LOG_INFO << "/events POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/events", Endpoints::createEventHandler);
    LOG_INFO << "/posts GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/posts", Endpoints::fetchPostsHandler);
    LOG_INFO << "/posts POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/posts", Endpoints::createPostHandler);
    

    LOG_INFO << "Server listening on localhost : 8080 ";
    apiServer->run();
}

RESTfulAPI RESTfulAPI::instance() 
{
    RESTfulAPI api;
    api.initEndpointHandlers();
    return api;
}