#include <backend/api/Endpoints.hpp>
#include <http/Status.hpp>
#include <util/logging/Log.hpp>
#include <JSON.hpp>
#include <crypto/RsaClient.hpp>
#include <crypto/RsaServer.hpp>
#include <crypto/StringEncoder.hpp>
#include <crypto/Hash.hpp>
#include <util/UUID.hpp>
#include <config.h>

Endpoints::Endpoints(/* args */)
{
}

Endpoints::~Endpoints()
{
}

indiepub::CredentialsController Endpoints::getCredentialsController()
{
    return indiepub::CredentialsController(CASS_CP, CASS_UN, CASS_PW, CASS_KS);
}

indiepub::UsersController Endpoints::getUsersController()
{
    return indiepub::UsersController(CASS_CP, CASS_UN, CASS_PW, CASS_KS);
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
    LOG_DEBUG << "signInHandler called with body: " << msg;
    try
    {
        std::string email;
        std::string pwHash;
        std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
        if (!msg.empty())
        {
            auto jsonObj = JSONObject(msg);
            for (const auto key : jsonObj.keys())
            {
                auto value = jsonObj[key];
                if (key == "email")
                {
                    std::vector<byte> encryptedData = StringEncoder::base64Decode(value.c_str());
                    byte *decryptedData = nullptr;
                    size_t decryptedLen = RsaServer::getInstance()->decrypt(encryptedData.data(), encryptedData.size(), decryptedData);
                    if (decryptedData && decryptedLen > 0 && decryptedLen < SIZE_MAX)
                    {
                        email = StringEncoder::bytesToString(decryptedData, decryptedLen);// std::string(reinterpret_cast<char *>(decryptedData), decryptedLen);
                        delete[] decryptedData;
                    }
                    else
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " Failed to decrypt email";
                        LOG_ERROR << "Failed to decrypt email";
                        response.setStatus(status);
                        response.setStatusMsg(errorMsg);
                        body->put("error", "Failed to decrypt email");
                        response.setBody(body->c_str());
                        return;
                    }
                    if (!isValidEmail(email))
                    {
                        int status = CODES::BAD_REQUEAST;
                        std::string errorMsg = Status(status).ss.str() + " " + email;
                        LOG_ERROR << "Invalid email format";
                        response.setStatus(status);
                        response.setStatusMsg(errorMsg);
                        body->put("error", "Invalid email format");
                        response.setBody(body->c_str());
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
                        std::vector<byte> pwdEnc = StringEncoder::base64Decode(part1.c_str());
                        if (pwdEnc.empty()) {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decode password";
                            LOG_ERROR << "Failed to decode password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            body->put("error", "Failed to decode password");
                            response.setBody(body->c_str());
                            return;
                        }
                        byte *decPwData = nullptr;
                        size_t pwLength = RsaServer::getInstance()->decrypt(pwdEnc.data(), pwdEnc.size(), decPwData);
                        if (pwLength == -1 || pwLength == SIZE_MAX)
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decrypt password";
                            LOG_ERROR << "Failed to decrypt password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            body->put("error", "Failed to decrypt password");
                            response.setBody(body->c_str());
                            if (decPwData) {
                                OPENSSL_free(decPwData);
                            }
                            return;
                        }
                        std::string password = StringEncoder::bytesToString(decPwData, pwLength);
                        OPENSSL_free(decPwData);
                    
                        std::vector<byte> signatureBytes = StringEncoder::base64Decode(part2);
                        bool isVerified = RsaClient::getInstance()->verify(password.c_str(), signatureBytes.data(), signatureBytes.size());
                        if (!isVerified)
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Password signature verification failed";
                            LOG_ERROR << "Password signature verification failed";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            body->put("error", "Password signature verification failed");
                            response.setBody(body->c_str());
                            return;
                        }

                        if (!isValidPassword(password))
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Weak password";
                            LOG_ERROR << "Invalid password format";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            body->put("error", "Weak password");
                            response.setBody(body->c_str());
                            return;
                        }
                        
                        // gets hash the value of the password
                        pwHash = hashing(password);
                    }
                }
            }
            LOG_DEBUG << jsonObj.dump(4);
        }
        if (email.empty() && pwHash.empty())
        {
            int status = CODES::BAD_REQUEAST;
            std::string errorMsg = Status(status).ss.str() + " password sent... not signed";
            LOG_ERROR << "Invalid password format";
            response.setStatus(status);
            response.setStatusMsg(errorMsg);
            body->put("error", "Invalid password format");
            response.setBody(body->c_str());
            LOG_DEBUG << response.getBody();
            return;
        }
        else
        {
            std::string token;
            indiepub::User user = getUsersController().getUserByEmail(email);
            
            if (user.user_id().empty())
            {
                response.setStatus(CODES::UNAUTHORIZE);
                response.setStatusMsg(Status(CODES::UNAUTHORIZE).ss.str());
                body->put("email", "Not registered");
                response.setBody(body->c_str());
            }
            else 
            {
                indiepub::Credentials creds = getCredentialsController().getCredentialsByUserId(user.user_id());

                std::string credsPwHash = creds.pw_hash();

                if(credsPwHash != pwHash) 
                {
                    response.setStatus(CODES::UNAUTHORIZE);
                    response.setStatusMsg(Status(CODES::UNAUTHORIZE).ss.str());
                    body->put("error", "wrong password");
                    response.setBody(body->c_str());
                }
                else
                {
                    response.setStatus(CODES::CREATED);
                    response.setStatusMsg(Status(CODES::CREATED).ss.str());
                    body->put("user", user.to_json());

                    std::string token = tokenGenerator(credsPwHash);
                    creds.set_auth_token(token);
                    if (getCredentialsController().insertCredentials(creds))
                    {
                        response.setStatus(CODES::CREATED);
                        response.setStatusMsg(Status(CODES::CREATED).ss.str());
                        std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
                        body->put("token", creds.auth_token());
                        body->put("user", user.to_json());
                        response.setBody(body->c_str());
                        LOG_DEBUG << response.getBody();
                    }
                    else
                    {
                        response.setStatus(CODES::CONFLICT);
                        response.setStatusMsg(Status(CODES::CONFLICT).ss.str());
                        LOG_ERROR << response.getStatusMsg();
                    }
                }
            }
            LOG_DEBUG << "id: " << email;
            LOG_DEBUG << "hash: " << pwHash;
        }
        
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
    }
    return;
}

void Endpoints::signUpHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    std::string msg = request.getBody();
    try
    {
        std::string email;
        std::string pwHash;
        std::string role;
        if (!msg.empty())
        {
            auto jsonObj = JSONObject(msg);
            for (const auto key : jsonObj.keys())
            {
                auto value = jsonObj[key];
                if (key == "email")
                {
                    std::vector<byte> encryptedData = StringEncoder::base64Decode(value.c_str());
                    byte *decryptedData = nullptr;
                    size_t decryptedLen = RsaServer::getInstance()->decrypt(encryptedData.data(), encryptedData.size(), decryptedData);
                    if (decryptedData && decryptedLen > 0)
                    {
                        email = StringEncoder::bytesToString(decryptedData, decryptedLen);// std::string(reinterpret_cast<char *>(decryptedData), decryptedLen);
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
                        std::string part1 = valueStr.substr(0, colonPos);
                        std::string part2 = valueStr.substr(colonPos + 1);
                        std::vector<byte> pwdEnc = StringEncoder::base64Decode(part1.c_str());
                        if (pwdEnc.empty()) {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decode password";
                            LOG_ERROR << "Failed to decode password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }
                        byte *decPwData = nullptr;
                        size_t pwLength = RsaServer::getInstance()->decrypt(pwdEnc.data(), pwdEnc.size(), decPwData);
                        if (pwLength == -1 || pwLength == SIZE_MAX)
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decrypt password";
                            LOG_ERROR << "Failed to decrypt password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }
                        std::string password = StringEncoder::bytesToString(decPwData, pwLength);
                        OPENSSL_free(decPwData);

                        std::vector<byte> signatureBytes = StringEncoder::base64Decode(part2);
                        bool isVerified = RsaClient::getInstance()->verify(password.c_str(), signatureBytes.data(), signatureBytes.size());
                        if (!isVerified)
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Password signature verification failed";
                            LOG_ERROR << "Password signature verification failed";
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
                        
                        // gets hash the value of the password
                        pwHash = hashing(password);
                    }
                }
                else if (key == "role")
                {
                    std::vector<byte> encryptedData = StringEncoder::base64Decode(value.c_str());
                    byte *decryptedData = nullptr;
                    size_t decryptedLen = RsaServer::getInstance()->decrypt(encryptedData.data(), encryptedData.size(), decryptedData);
                    if (decryptedData && decryptedLen > 0)
                    {
                        role = StringEncoder::bytesToString(decryptedData, decryptedLen);
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
                }
            }
            LOG_DEBUG << jsonObj.dump(4);
        }
        if (email.empty() && pwHash.empty() && role.empty())
        {
            int status = CODES::BAD_REQUEAST;
            std::string errorMsg = Status(status).ss.str() + " signup error";
            LOG_ERROR << "Invalid password format";
            response.setStatus(status);
            response.setStatusMsg(errorMsg);
            return;
        }
        else
        {
            indiepub::User user = getUsersController().getUserByEmail(email);
            if (user.user_id().empty())
            {
                user.user_id(UUID::random());
                user.email(email);
                user.role(role);
                user.name(user.user_id());
                user.created_at(std::time(nullptr));
                if (getUsersController().insertUser(user)) 
                {
                    std::string token = tokenGenerator(pwHash);

                    indiepub::Credentials creds(
                            user.user_id(), 
                            token, 
                            pwHash
                        );
                    if (getCredentialsController().insertCredentials(creds))
                    {
                        response.setStatus(CODES::CREATED);
                        response.setStatusMsg(Status(CODES::CREATED).ss.str());
                        std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
                        body->put("token", creds.auth_token());
                        body->put("user", user.to_json());
                        response.setBody(body->c_str());
                        response.addHeader("Content-Type", "application/json");
                        LOG_DEBUG << response.getBody();
                    }
                    else 
                    {
                        response.setStatus(CODES::CONFLICT);
                        response.setStatusMsg(Status(CODES::CONFLICT).ss.str());
                        LOG_ERROR << response.getStatusMsg();
                    }
                    
                }
            }
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
    }
}

std::string Endpoints::tokenGenerator(std::string &pwHash)
{
    byte *tokenBytes = nullptr;
    size_t tokenLength = RsaServer::getInstance()->sign(pwHash.c_str(), tokenBytes, "");
    std::string authToken = StringEncoder::bytesToHex(tokenBytes, tokenLength);
    return authToken;
}

std::string Endpoints::hashing(std::string &password)
{
    std::vector<byte> pwEnc = StringEncoder::stringToBytes(password);
    byte *md = nullptr;
    
    std::string pwHash = StringEncoder::bytesToHex(Hash::sha256(pwEnc.data()), pwEnc.size());
    delete[] md;
    return pwHash;
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
