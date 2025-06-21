#include <backend/api/Endpoints.hpp>
#include <http/Status.hpp>
#include <util/logging/Log.hpp>
#include <JSON.hpp>
#include <crypto/RsaClient.hpp>
#include <crypto/RsaServer.hpp>
#include <crypto/StringEncoder.hpp>
#include "config.h"

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
    // Check for presence of at least one digit, one uppercase letter, and one lowercase letter
    bool hasDigit = false;
    bool hasUpper = false;
    bool hasLower = false;
    bool hasSpecial = false;
    const std::string specialChars = "!@#$%^&*()-_=+[]{}|;:',.<>?/";
    for (char c : password)
    {
        if (isdigit(c))
            hasDigit = true;
        else if (isupper(c))
            hasUpper = true;
        else if (islower(c))
            hasLower = true;
        else if (specialChars.find(c) != std::string::npos)
            hasSpecial = true;
    }
    // Check if all conditions are met
    if (!hasDigit || !hasUpper || !hasLower || !hasSpecial)
    {
        return false;
    }

    // Check for minimum length
    return password.length() >= 10;
}

void Endpoints::signInHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    std::string msg = request.getBody();
    try
    {
        std::string email;
        std::string password;
        if (!msg.empty())
        {
            auto jsonObj = JSONObject(msg);
            for (const auto key : jsonObj.keys())
            {
                auto value = jsonObj[key];
                if (key == "email")
                {
                    std::vector<byte> encryptedData = StringEncoder::hexToBytes(value.c_str());
                    byte *decryptedData = nullptr;
                    size_t decryptedLen = RsaServer::getInstance()->decrypt(encryptedData.data(), encryptedData.size(), decryptedData);
                    std::string email;
                    if (decryptedData && decryptedLen > 0)
                    {
                        email = std::string(reinterpret_cast<char *>(decryptedData), decryptedLen);
                        delete[] decryptedData;
                    }
                    else
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " Failed to decrypt email";
                        LOG_ERROR << "Failed to decrypt email";
                        response.setStatus(status);
                        response.setStatusMsg(errorMsg);
                        return;
                    }
                    if (!isValidEmail(email))
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " " + email;
                        LOG_ERROR << "Invalid email format";
                        response.setStatus(status);
                        response.setStatusMsg(errorMsg);
                        return;
                    }
                }
                else if (key == "password")
                {
                    std::string valueStr = value.c_str();
                    size_t colonPos = valueStr.find(':');
                    std::vector<std::string> parts;
                    if (colonPos != std::string::npos)
                    {
                        // Split into two parts based on ':'
                        std::string part1 = valueStr.substr(0, colonPos);
                        std::string part2 = valueStr.substr(colonPos + 1);
                        LOG_DEBUG << "part1: " << part1;
                        LOG_DEBUG << "part2: " << part2;
                        std::vector<byte> pwdEnc = StringEncoder::base64Decode(part1.c_str());
                        byte *decPwData = nullptr;
                        size_t pwLength = RsaServer::getInstance()->decrypt(pwdEnc.data(), pwdEnc.size(), decPwData);
                        if (pwLength == 1024)
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decrypt password";
                            LOG_ERROR << "Failed to decrypt password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }
                        std::string pwStr = StringEncoder::bytesToString(decPwData, pwLength);
                        LOG_DEBUG << "part1: " << pwStr;
                        LOG_DEBUG << "signed part1: " << part2;
                        // Here, part1 is the password and part2 is the signature                        
                        // You can choose which part to use as password, or combine them as needed
                        // Here, as an example, we concatenate them with a colon

                        // std::vector<byte> passwordBytes = StringEncoder::hexToBytes(part1.c_str());
                        std::vector<byte> signatureBytes = StringEncoder::base64Decode(part2.c_str());
                        bool isVerified = RsaClient::getInstance()->verify(pwStr.c_str(), signatureBytes.data(), signatureBytes.size());
                        if (!isVerified)
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Password signature verification failed";
                            LOG_ERROR << "Password signature verification failed";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }
                        std::vector<byte> encryptedPassword = StringEncoder::stringToBytes(part1.c_str());
                        byte *decryptedPassword = nullptr;
                        size_t decryptedPasswordLen = RsaClient::getInstance()->decrypt(
                            encryptedPassword.data(), encryptedPassword.size(), decryptedPassword);

                        std::string password;
                        if (decryptedPassword && decryptedPasswordLen > 0)
                        {
                            password = std::string(reinterpret_cast<char *>(decryptedPassword), decryptedPasswordLen);
                            delete[] decryptedPassword;
                        }
                        else
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decrypt password";
                            LOG_ERROR << "Failed to decrypt password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }
                        if (!isValidPassword(password))
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Weak password";
                            LOG_ERROR << "Invalid password format";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }
                        password = part1.c_str(); // Use the first part as the password
                    }
                    else
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " password sent... not signed";
                        LOG_ERROR << "Invalid password format";
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
    catch (const std::exception &e)
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
