
#include "config.h" 
#include <api/Client.hpp>
#include <crypto/RsaServer.hpp>
#include <crypto/RsaClient.hpp>
#include <crypto/StringEncoder.hpp>
#include <memory>
#include <JSON.hpp>
#include <util/logging/Log.hpp>

std::shared_ptr<ApiClient> apiClient;
std::shared_ptr<AuthCrypto> rsaServer = RsaServer::getInstance();
std::shared_ptr<AuthCrypto> rsaClient = RsaClient::getInstance();
std::string baseUrl = "http://localhost:8008";

std::string email = "test@indiepub.com";
std::string password = "Passw0rd!!";

std::string name = "Tester Indiepub";
std::string role = "fan";


bool isApiUp() {
    try
    {
        if (apiClient == nullptr)
        {
            apiClient = std::make_shared<ApiClient>();
        }
        
        std::string testUrl = baseUrl + "/test";

        std::unordered_map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};
        auto response = apiClient->get(testUrl, "", headers);
        LOG_INFO << response.getBody(); // Ensure we read the body to avoid issues with the connection
        return response.getStatus() == 200;
    }
    catch (const std::exception &e)
    {
        std::cerr << "API is down: " << e.what() << '\n';
        return false;
    }
}

void testSginInHandler()
{
    try
    {
        if (apiClient == nullptr)
        {
            apiClient = std::make_shared<ApiClient>();
        }
        std::vector<byte> emailData = StringEncoder::stringToBytes(email);
        byte *emailEnc = nullptr;
        if (rsaServer->isPublicKeyRsa()) 
        {
            rsaServer->loadPublicKey();
        }
        else
        {
            throw std::runtime_error("Public key is not RSA");
        }
        size_t enc_len = RsaServer::getInstance()->encrypt(emailData.data(), emailEnc);
        std::string emailEncStr = StringEncoder::base64Encode(emailEnc, enc_len);

        std::vector<byte> pwData = StringEncoder::stringToBytes(password);
        byte *pwEnc = nullptr;
        size_t pw_len = RsaServer::getInstance()->encrypt(pwData.data(), pwEnc);
        std::string pwEncStr = StringEncoder::base64Encode(pwEnc, pw_len);

        byte *sign = nullptr;
        size_t sign_len = RsaClient::getInstance()->sign(password.c_str(), sign, "");
        std::string signature = StringEncoder::base64Encode(sign, sign_len);

        std::unique_ptr<JSONObject> obj = std::make_unique<JSONObject>();
        obj->put("email", emailEncStr);
        obj->put("password", pwEncStr + ":" + signature);

        std::string signinUrl = baseUrl + "/login";

        std::unordered_map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};
        LOG_DEBUG << "Signin URL: " << signinUrl;
        LOG_DEBUG << "Signin Object: " << obj->dump();
        auto response = apiClient->post(signinUrl, obj->c_str(), headers);
        LOG_DEBUG << "signin status response: " << response.getStatus();
        LOG_DEBUG << "signin body response: " << response.getBody();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

void testSginUpHandler()
{
    try
    {
        if (apiClient == nullptr)
        {
            apiClient = std::make_shared<ApiClient>();
        }
        std::vector<byte> emailData = StringEncoder::stringToBytes(email);
        byte *emailEnc = nullptr;

        size_t enc_len = RsaServer::getInstance()->encrypt(emailData.data(), emailEnc);
        std::string emailEncStr = StringEncoder::base64Encode(emailEnc, enc_len);

        std::vector<byte> pwData = StringEncoder::stringToBytes(password);
        byte *pwEnc = nullptr;
        size_t pw_len = RsaServer::getInstance()->encrypt(pwData.data(), pwEnc);
        std::string pwEncStr = StringEncoder::base64Encode(pwEnc, pw_len);

        byte *sign = nullptr;
        size_t sign_len = RsaClient::getInstance()->sign(password.c_str(), sign, "");
        std::string signature = StringEncoder::base64Encode(sign, sign_len);

        std::vector<byte> roleData = StringEncoder::stringToBytes(role);
        byte *roleEnc = nullptr;
        size_t role_len = RsaServer::getInstance()->encrypt(roleData.data(), roleEnc);
        std::string roleEncStr = StringEncoder::base64Encode(roleEnc, role_len);

        std::unique_ptr<JSONObject> obj = std::make_unique<JSONObject>();
        obj->put("email", emailEncStr);
        obj->put("role", roleEncStr);
        obj->put("password", pwEncStr + ":" + signature);

        std::string signupUrl = baseUrl + "/signup";

        LOG_DEBUG << "Signup URL: " << signupUrl;
        LOG_DEBUG << "Signup Object: " << obj->dump();

        std::unordered_map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};
        auto response = apiClient->post(signupUrl, obj->c_str(), headers);
        LOG_DEBUG << "signup status response: " << response.getStatus();
        LOG_DEBUG << "signup body response: " << response.getBody();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int main()
{
    if (!isApiUp())
    {
        std::cerr << "API is not running. Please start the server first." << std::endl;
        return EXIT_FAILURE;
    }
    // testSginUpHandler();
    testSginInHandler();
    return EXIT_SUCCESS;
}
