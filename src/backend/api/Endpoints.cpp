#include <backend/api/Endpoints.hpp>
#include <http/Status.hpp>
#include <util/logging/Log.hpp>
#include <JSON.hpp>

Endpoints::Endpoints(/* args */)
{
}

Endpoints::~Endpoints()
{
}

void Endpoints::signInHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "SignInHandler called";
    std::string msg = request.getBody();
    try
    {        
        auto jsonObj = JSONObject::parse(msg);
        LOG_DEBUG << jsonObj->dump(4);
        response.setBody("Hello, World!");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
    }
}

void Endpoints::signUpHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "SignUpHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::fetchEventsHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "getFetchEventsHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::createEventHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "createEventHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::fetchPostsHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "fetchPostsHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::createPostHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "createPostHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}
