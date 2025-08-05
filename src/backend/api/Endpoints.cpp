#include <backend/api/Endpoints.hpp>
#include <http/Status.hpp>
#include <util/logging/Log.hpp>
#include <JSON.hpp>
#include <crypto/RsaClient.hpp>
#include <crypto/RsaServer.hpp>
#include <crypto/StringEncoder.hpp>
#include <crypto/Hash.hpp>
#include <util/UUID.hpp>
#include <util/String.hpp>
#include <config.h>
#include <backend/IndieBackModels.hpp>
#include <ctime>

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

indiepub::EventController Endpoints::getEventController()
{
    return indiepub::EventController(CASS_CP, CASS_UN, CASS_PW, CASS_KS);
}

indiepub::VenuesController Endpoints::getVenuesController()
{
    return indiepub::VenuesController(CASS_CP, CASS_UN, CASS_PW, CASS_KS);
}

indiepub::VenueMembersController Endpoints::getVenueMembersController()
{
    return indiepub::VenueMembersController(CASS_CP, CASS_UN, CASS_PW, CASS_KS);
}

bool Endpoints::validateTokenAndId(const HttpRequest &request, HttpResponse &response, Path *path, indiepub::Credentials &creds, indiepub::User &user)
{
    auto headers = request.getHeaders();
    std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
    if (headers.find("authorization") == headers.end())
    {
        response.setStatus(CODES::UNAUTHORIZED);
        response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
        body->put("error", "Token not provided");
        response.setBody(body->c_str());
        return false;
    }
    else
    {
        std::string auth = headers["authorization"];
        if (auth.empty() || auth.find("Bearer ") != 0)
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "Invalid token format");
            response.setBody(body->c_str());
            return false;
        }
        std::string token = auth.substr(7); // Remove "Bearer " prefix
        if (token[token.size()-1] == '\r' || token[token.size()-1] == '\n')
            token = token.substr(0, token.size()-1);
        creds = getCredentialsController().getCredentialsByAuthToken(token);
        if (creds.auth_token().empty())
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "Invalid token");
            response.setBody(body->c_str());
            return false;
        }
        std::string xuserId = headers["x-user-id"];
        if(xuserId[xuserId.size()-1] == '\r' || xuserId[xuserId.size()-1] == '\n') 
        {
            xuserId = xuserId.substr(0, xuserId.size() - 1);
        }
        if (xuserId.empty()) {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "User ID not provided");
            response.setBody(body->c_str());
            return false;
        }
        if (xuserId != creds.user_id())
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "User ID does not match token");
            response.setBody(body->c_str());
            return false;
        }
        user = getUsersController().getUserById(creds.user_id());
        if (user.user_id().empty())
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "User not found");
            response.setBody(body->c_str());
            return false;
        }
    }
    return true;
}

std::string Endpoints::hashing(std::string &password)
{
    std::vector<byte> pwEnc = StringEncoder::stringToBytes(password);
    byte *md = nullptr;

    std::string hash = StringEncoder::bytesToHex(Hash::sha256(pwEnc.data()), pwEnc.size());
    delete[] md;
    return hash;
}

std::string Endpoints::tokenGenerator(std::string &pwHash)
{
    byte *tokenBytes = nullptr;
    size_t tokenLength = RsaServer::getInstance()->sign(pwHash.c_str(), tokenBytes, "");
    std::string authToken = StringEncoder::bytesToHex(tokenBytes, tokenLength);
    return authToken;
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

std::string Endpoints::decryptMessage(const std::string &value)
{
    try
    {
        std::string result = "";
        std::vector<byte> encryptedData = StringEncoder::base64Decode(value.c_str());
        byte *decryptedData = nullptr;
        size_t decryptedLen = RsaServer::getInstance()->decrypt(encryptedData.data(), encryptedData.size(), decryptedData);
        if (decryptedData && decryptedLen > 0 && decryptedLen < SIZE_MAX)
        {
            result = StringEncoder::bytesToString(decryptedData, decryptedLen);
            delete[] decryptedData;
        }
        else
        {
            throw std::runtime_error("Decryption error");
        }
        return result;
    }
    catch (std::runtime_error &ex) 
    {
        throw std::runtime_error(ex.what());
    }
}

void Endpoints::signInHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    std::string msg = request.getBody();
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
                    email = decryptMessage(value.c_str());
                    if (email.empty())
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
                        LOG_DEBUG << part1 << " : " << part1.size();

                        std::string password = decryptMessage(part1);
                        
                        if (password.empty())
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decrypt password";
                            LOG_ERROR << "Failed to decrypt password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            body->put("error", "Failed to decrypt password");
                            response.setBody(body->c_str());
                            return;
                        }
                        

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
                response.setStatus(CODES::UNAUTHORIZED);
                response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
                body->put("email", "Not registered");
                response.setBody(body->c_str());
            }
            else
            {
                indiepub::Credentials creds = getCredentialsController().getCredentialsByUserId(user.user_id());
                if (creds.pw_hash() != pwHash)
                {
                    response.setStatus(CODES::UNAUTHORIZED);
                    response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
                    body->put("error", "wrong password");
                    response.setBody(body->c_str());
                }
                else
                {
                    response.setStatus(CODES::CREATED);
                    response.setStatusMsg(Status(CODES::CREATED).ss.str());
                    std::string token = tokenGenerator(pwHash);
                    creds.set_auth_token(token);
                    if (getCredentialsController().insertCredentials(creds))
                    {
                        response.setStatus(CODES::CREATED);
                        response.setStatusMsg(Status(CODES::CREATED).ss.str());
                        std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
                        body->put("token", creds.auth_token());
                        body->put("user_id", user.user_id());
                        body->put("email", user.email());
                        body->put("role", user.role());
                        body->put("name", user.name());
                        body->put("created_at", indiepub::timestamp_to_string(user.created_at()));
                        body->put("bio", user.bio());
                        body->put("profile_picture", user.profile_picture());
                        std::string socialLinks;
                        for (const auto& link : user.social_links()) {
                            socialLinks += link + ",";
                        }
                        if (!socialLinks.empty()) {
                            socialLinks.pop_back(); // Remove the trailing comma
                        }
                        body->put("social_links", socialLinks);;
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
    LOG_DEBUG << "signUpHandler: " << msg;
    try
    {
        std::string email;
        std::string pwHash;
        std::string role;
        if (!msg.empty())
        {
            std::unique_ptr<JSONObject> jsonObj = std::make_unique<JSONObject>(msg);
            for (const auto key : jsonObj->keys())
            {
                auto value = jsonObj->get(key);
                if (key == "email")
                {
                    email = decryptMessage(value.c_str());
                    if (email.empty())
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
                        std::string password = decryptMessage(part1);
                        if (password.empty())
                        {
                            int status = CODES::BAD_REQUEAST;
                            std::string errorMsg = Status(status).ss.str() + " Failed to decrypt password";
                            LOG_ERROR << "Failed to decrypt password";
                            response.setStatus(status);
                            response.setStatusMsg(errorMsg);
                            return;
                        }

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
                    role = decryptMessage(value.c_str());
                    if (role.empty())
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
            LOG_DEBUG << jsonObj->dump(4);
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
                auto at_pos = email.find('@');
                std::string uname = (at_pos != std::string::npos) ? email.substr(0, at_pos) : email;
                user.name(uname); // Use the part before '@' as the name
                user.created_at(std::time(nullptr));
                if (getUsersController().insertUser(user))
                {
                    std::string token = tokenGenerator(pwHash);

                    indiepub::Credentials creds(
                        user.user_id(),
                        token,
                        pwHash);
                    if (getCredentialsController().insertCredentials(creds))
                    {
                        response.setStatus(CODES::CREATED);
                        response.setStatusMsg(Status(CODES::CREATED).ss.str());
                        std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
                        body->put("token", creds.auth_token());
                        body->put("user_id", user.user_id());
                        body->put("email", user.email());
                        body->put("role", user.role());
                        body->put("name", user.name());
                        body->put("created_at", indiepub::timestamp_to_string(user.created_at()));
                        body->put("bio", user.bio());
                        body->put("profile_picture", user.profile_picture());
                        JSONArray socialLinksArray;
                        for (const auto& link : user.social_links()) {
                            socialLinksArray.add(link);
                        }
                        body->put("social_links", socialLinksArray);
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
        }
    }
    catch (const std::exception &e)
    {
        LOG_ERROR << e.what();
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
    }
}

\
void Endpoints::fetchUserInfoHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    indiepub::Credentials creds;
    indiepub::User user;
    if (validateTokenAndId(request, response, path, creds, user))
    {
        std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>(user.to_json());
        body->put("token", creds.auth_token());
        response.setStatus(CODES::OK);
        response.setStatusMsg(Status(CODES::OK).ss.str());
        response.setBody(body->c_str());
    }
}

void Endpoints::validateHeaders(const HttpRequest &request, HttpResponse &response, Path *path)
{
    indiepub::Credentials creds;
    indiepub::User user;
    auto headers = request.getHeaders();
    std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>();
    if (headers.find("authorization") == headers.end())
    {
        response.setStatus(CODES::UNAUTHORIZED);
        response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
        body->put("error", "Token not provided");
        response.setBody(body->c_str());
        return;
    }
    else
    {
        std::string auth = headers["authorization"];
        if (auth.empty() || auth.find("Bearer ") != 0)
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "Invalid token format");
            response.setBody(body->c_str());
            return;
        }
        std::string token = auth.substr(7); // Remove "Bearer " prefix
        creds = getCredentialsController().getCredentialsByAuthToken(token);
        if (creds.auth_token().empty())
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "Invalid token");
            response.setBody(body->c_str());
            return;
        }
        std::string xuserId = headers["x-user-id"];
        if (xuserId.empty()) {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "User ID not provided");
            response.setBody(body->c_str());
            return;
        }
        if (xuserId != creds.user_id())
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "User ID does not match token");
            response.setBody(body->c_str());
            return;
        }
        user = getUsersController().getUserById(creds.user_id());
        if (user.user_id().empty())
        {
            response.setStatus(CODES::UNAUTHORIZED);
            response.setStatusMsg(Status(CODES::UNAUTHORIZED).ss.str());
            body->put("error", "User not found");
            response.setBody(body->c_str());
            return;
        }
        body = std::make_unique<JSONObject>(user.to_json());
        body->put("token", creds.auth_token());
        response.setStatus(CODES::OK);
        response.setStatusMsg(Status(CODES::OK).ss.str());
        response.setBody(body->c_str());
    }
    
}


void Endpoints::fetchEventsHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    indiepub::Credentials creds;
    indiepub::User user;
    std::string venueId = "";
    indiepub::Venue venue;
    std::unique_ptr<JSONArray> events = std::make_unique<JSONArray>();
    LOG_DEBUG << "getFetchEventsHandler called";
    if (validateTokenAndId(request, response, path, creds, user))
    {
        auto allEvents = getEventController().getAllEvents();
        
        for (const auto &event : allEvents)
        {
            if (venueId != event.venue_id())
            {
                venueId = event.venue_id();
                venue = getVenuesController().getVenueById(venueId);
                if (venue.venue_id().empty())
                {
                    LOG_ERROR << "Venue not found for event: " << event.event_id();
                    continue; // Skip this event if venue is not found
                }
                else
                {
                    std::unique_ptr<JSONObject> eventObj = std::make_unique<JSONObject>();
                    eventObj->put("event_id", event.event_id());
                    eventObj->put("name", event.name());
                    eventObj->put("date", indiepub::timestamp_to_string(event.date()));
                    eventObj->put("location", "`" + venue.name() + "` " + venue.location());
                    eventObj->put("ticket_price", event.price());
                    eventObj->put("capacity", venue.capacity());
                    eventObj->put("creator_id", event.creator_id());
                    eventObj->put("sold", event.sold());
                    events->add(JSON(eventObj->dump(4)));
                }
            }
        }
        response.setBody(events->c_str());
        response.setStatus(200);
    }
    else
    {
        auto oneWeekEvents = getEventController().getOneWeekEvents(time(nullptr));
        for (const auto &event : oneWeekEvents)
        {
            if (venueId != event.venue_id())
            {
                venueId = event.venue_id();
                venue = getVenuesController().getVenueById(venueId);
                if (venue.venue_id().empty())
                {
                    LOG_ERROR << "Venue not found for event: " << event.event_id();
                    continue; // Skip this event if venue is not found
                }
                else
                {
                    std::unique_ptr<JSONObject> eventObj = std::make_unique<JSONObject>();
                    eventObj->put("event_id", event.event_id());
                    eventObj->put("name", event.name());
                    eventObj->put("date", indiepub::timestamp_to_string(event.date()));
                    eventObj->put("location", "`" + venue.name() + "` " + venue.location());
                    eventObj->put("ticket_price", event.price());
                    eventObj->put("capacity", venue.capacity());
                    eventObj->put("creator_id", event.creator_id());
                    eventObj->put("sold", event.sold());
                    events->add(JSON(eventObj->dump(4)));
                }
            }
        }
        response.setBody(events->c_str());
        response.setStatus(200);
    }
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
    LOG_DEBUG << "createPostHanPdler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::updateUserInfoHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    indiepub::Credentials creds;
    indiepub::User user;
    
    try
    {
        bool result = false;
        if (validateTokenAndId(request, response, path, creds, user))
        {
            std::string requestStr = request.getBody();
            LOG_DEBUG << requestStr;
            std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(requestStr);
            
            std::string name = decryptMessage(jsonObject->get("name").c_str());
            std::string bio = jsonObject->get("bio").c_str();
            std::string links = jsonObject->get("social_links").c_str();
            std::string profilePicture = jsonObject->get("profile_picture").c_str();
            std::regex rx(",");
            std::vector<std::string> socialLinksVector = String::tokenize(links, rx);
            std::vector<std::string> socialLinks;
            for(auto& link: socialLinksVector)
            {
                std::string socialLink = decryptMessage(link);
                socialLinks.push_back(String::trim(socialLink));
            }
            user.social_links(socialLinks);
            user.name(name);
            user.bio(bio);
            user.profile_picture(profilePicture);
            result = getUsersController().updateUser(user);
        }
        if (result) 
        {
            response.setStatus(CODES::ACCPETED);
            response.setStatusMsg(Status(CODES::ACCPETED).ss.str());
        } 
        else 
        {
            response.setStatus(CODES::NOT_ACCEPTABLE);
            response.setStatusMsg(Status(CODES::NOT_ACCEPTABLE).ss.str());
            response.setBody("{\"error\": \"failed to save venue information\"}");
        }
    } 
    catch (std::runtime_error &ex)
    {
        LOG_ERROR << ex.what();
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
        response.setBody("{\"error\": \"someting went wrong in the server side\"}");
    }
}


void Endpoints::addBandProfileHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "addBandProfileHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::fetchBandProfileHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "fetchBandProfileHandler called";
    response.setBody("Hello, World!");
    response.setStatus(200);
}

void Endpoints::addVenueProfileHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "addVenueProfileHandler called";
    indiepub::Credentials creds;
    indiepub::User user;

    try
    {
        bool result = false;
    
        if (validateTokenAndId(request, response, path, creds, user))
        {
            std::string requestStr = request.getBody();
            LOG_DEBUG << requestStr;
            std::unique_ptr<JSONObject> jsonObject = std::make_unique<JSONObject>(requestStr);
            
            
            std::string venueId = jsonObject->get("venue_id").c_str().empty()? UUID::random() : decryptMessage(jsonObject->get("venue_id").c_str());
            std::time_t createdAt = indiepub::string_to_timestamp(jsonObject->get("created_at").str());
            long capacity = std::stol(jsonObject->get("capacity").str());
            std::string name = decryptMessage(jsonObject->get("name").c_str());
            std::string location = decryptMessage(jsonObject->get("location").c_str());
            std::string userId = decryptMessage(jsonObject->get("user_id").c_str());
            std::string memberType = decryptMessage(jsonObject->get("member_type").c_str());
            
            indiepub::Venue venue = getVenuesController().getVenueById(venueId);
            indiepub::VenueMembers venueMember = getVenueMembersController().getVenueMemberByUserId(userId);

            if (!venueMember.venue_id().empty() && venueMember.user_id() == userId && venueMember.is_active())
            {
                LOG_DEBUG << "Venue already exists for user: " << userId;
                response.setStatus(CODES::ACCPETED);
                response.setStatusMsg(Status(CODES::ACCPETED).ss.str());
                std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>(venue.to_json());
                body->put("user_id", user.user_id());
                body->put("created_at", indiepub::timestamp_to_string(venue.created_at()));
                response.setBody(body->c_str());
                return;
            }

            if (!venue.venue_id().empty())
            {   
                indiepub::VenueMembers venueMember(
                    venue.venue_id(),
                    userId,
                    memberType,
                    true,
                    std::time(nullptr));
            
                venue = indiepub::Venue(
                    venueId.empty() ? UUID::random() : venueId,
                    user.user_id(),
                    name,
                    location,
                    capacity,
                    createdAt);
                result = getVenuesController().updateVenue(venue);
                result &= getVenueMembersController().updateVenueMember(venueMember);
            }
            else 
            {
                indiepub::VenueMembers venueMember(
                    venueId,
                    userId,
                    memberType,
                    true,
                    std::time(nullptr));
            
                venue = indiepub::Venue(
                    UUID::random(),
                    user.user_id(),
                    name,
                    location,
                    capacity,
                    createdAt);
                result = getVenuesController().insertVenue(venue);
                result &= getVenueMembersController().insertVenueMember(venueMember);
            }
            if (result)
            {
                response.setStatus(CODES::ACCPETED);
                response.setStatusMsg(Status(CODES::ACCPETED).ss.str());
                response.setBody(venue.to_json().c_str());
            }
            else
            {
                response.setStatus(CODES::NOT_ACCEPTABLE);
                response.setStatusMsg(Status(CODES::NOT_ACCEPTABLE).ss.str());
                response.setBody("{\"error\": \"failed to save venue information\"}");
            }
        }
    }
    catch (std::runtime_error &ex)
    {
        LOG_ERROR << ex.what();
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
        response.setBody("{\"error\": \"someting went wrong in the server side\"}");
        return;
    }
}

void Endpoints::fetchVenueProfileHandler(const HttpRequest &request, HttpResponse &response, Path *path)
{
    LOG_DEBUG << "fetchVenueProfileHandler called";
    indiepub::Credentials creds;
    indiepub::User user;

    try
    {
        bool result = false;
    
        if (validateTokenAndId(request, response, path, creds, user))
        {
            indiepub::VenueMembers vm = getVenueMembersController().getVenueMemberByUserId(user.user_id());
            if (vm.venue_id().empty())
            {
                response.setStatus(CODES::NOT_FOUND);
                response.setStatusMsg(Status(CODES::NOT_FOUND).ss.str());
                response.setBody("{\"error\": \"Venue not found for user\"}");
                return;
            }
            indiepub::Venue venue = getVenuesController().getVenueById(vm.venue_id());
            if (!venue.venue_id().empty())
            {
                result = true;
            }
            
            if (result)
            {
                response.setStatus(CODES::ACCPETED);
                response.setStatusMsg(Status(CODES::ACCPETED).ss.str());
                std::unique_ptr<JSONObject> body = std::make_unique<JSONObject>(venue.to_json());
                response.setBody(body->c_str());
            }
            else
            {
                response.setStatus(CODES::NOT_ACCEPTABLE);
                response.setStatusMsg(Status(CODES::NOT_ACCEPTABLE).ss.str());
                response.setBody("{\"error\": \"failed to save venue information\"}");
            }
        }
    }
    catch (std::runtime_error &ex)
    {
        LOG_ERROR << ex.what();
        response.setStatus(CODES::INTERNAL_SERVER_ERROR);
        response.setStatusMsg(Status(CODES::INTERNAL_SERVER_ERROR).ss.str());
        response.setBody("{\"error\": \"someting went wrong in the server side\"}");
        return;
    }
}