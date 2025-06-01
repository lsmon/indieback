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

bool Endpoints::isValidEmail(const std::string &email)
{
    // Basic check for presence of '@' and '.'
    auto at_pos = email.find('@');
    auto dot_pos = email.find('.', at_pos);
    return at_pos != std::string::npos && dot_pos != std::string::npos && at_pos > 0 && dot_pos > at_pos + 1;
}

bool Endpoints::isValidPassword(const std::string &password)
{
    // Example validation: password must be at least 8 characters long
    if (password.empty())
    {
        return false;
    }

    // Check for minimum length
    return password.length() >= 10;
}


void Endpoints::signInHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "SignInHandler called";
    std::string msg = request.getBody();
    LOG_DEBUG << msg;
    try
    {
        if (!msg.empty())
        {
            auto jsonObj = JSONObject(msg);
            for (const auto key : jsonObj.keys())
            {
                auto value = jsonObj[key];
                LOG_DEBUG << key << ": " << value;
                if (key == "email")
                {
                    std::string email = value.c_str();
                    if (!isValidEmail(email))
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " " + email;
                        LOG_DEBUG << "Invalid email format";
                        response.setStatus(status);
                        response.setStatusMsg(errorMsg);
                        return;
                    }
                }
                else if (key == "password")
                {
                    std::string password = value.c_str();
                    if (!isValidPassword(password))
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " Weak password";
                        LOG_DEBUG << "Invalid password format";
                        response.setStatus(status);
                        response.setStatusMsg(errorMsg);
                        return;
                    }
                }
                else
                {
                    int status = CODES::UNAUTHORIZE;
                    std::string errorMsg = Status(status).ss.str() + " Unknown key: " + key;            
                    response.setStatus(status);
                    response.setStatusMsg(errorMsg);
                    return;
                }
            }
            LOG_DEBUG << jsonObj.dump(4);
        }
        response.setBody("Hello, World!");
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
    }
    return;
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
