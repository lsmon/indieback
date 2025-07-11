#include <backend/api/RESTfulAPI.hpp>
#include <backend/api/Endpoints.hpp>
#include <util/logging/Log.hpp>

RESTfulAPI::RESTfulAPI()
{
    apiServer = std::make_unique<HttpServer>("localhost", "8008", 1024, 4);
}

void RESTfulAPI::initEndpointHandlers() {
    LOG_INFO << "Mapping endpoints";
    LOG_INFO << "/validate POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/validate", Endpoints::validateHeaders);
    LOG_INFO << "/user/info GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/user/info", Endpoints::fetchUserInfoHandler);
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
    LOG_INFO << "/user/profile GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/user/profile", Endpoints::fetchUserInfoHandler);

    LOG_INFO << "/user/profile UPDATE";
    apiServer->setHttpHandler(HttpMethod::GET, "/user/profile", Endpoints::updateUserInfoHandler);

    LOG_INFO << "/venue/profile POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/venue/profile", Endpoints::addVenueProfileHandler);
    LOG_INFO << "/venue/profile GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/venue/profile", Endpoints::fetchVenueProfileHandler);
    LOG_INFO << "/band/profile POST";
    apiServer->setHttpHandler(HttpMethod::POST, "/band/profile", Endpoints::addBandProfileHandler);
    LOG_INFO << "/band/profile GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/band/profile", Endpoints::fetchBandProfileHandler);
    LOG_INFO << "/tests GET";
    apiServer->setHttpHandler(HttpMethod::GET, "/test", [](const HttpRequest &request, HttpResponse &response, Path *path) {
        response.setBody("Hello, World!");
        response.setStatus(200);
    });

    LOG_INFO << "Server listening on localhost : 8008 ";
    apiServer->run();
}

RESTfulAPI RESTfulAPI::instance() 
{
    RESTfulAPI api;
    api.initEndpointHandlers();
    return api;
}