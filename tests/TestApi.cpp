
#include "config.h" 
#include <api/Client.hpp>
#include <crypto/RsaServer.hpp>
#include <crypto/RsaClient.hpp>
#include <crypto/StringEncoder.hpp>
#include <memory>
#include <JSON.hpp>

std::shared_ptr<ApiClient> apiClient;
std::shared_ptr<AuthCrypto> rsaServer = RsaServer::getInstance();
std::shared_ptr<AuthCrypto> rsaClient = RsaClient::getInstance();
std::string baseUrl = "http://localhost:8008";

std::string email = "test@indiepub.com";
std::string password = "Passw0rd!!";

std::string name = "Tester Indiepub";
std::string role = "fan";

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

        std::string login = baseUrl + "/login";

        std::unordered_map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};
        auto response = apiClient->post(baseUrl, obj->c_str(), headers);
        std::cout << "signin status response: " << response.getStatus() << std::endl;
        std::cout << "signin body response: " << response.getBody() << std::endl;
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

        std::string login = baseUrl + "/signup";

        std::unordered_map<std::string, std::string> headers = {
            {"Content-Type", "application/json"}};
        auto response = apiClient->post(baseUrl, obj->c_str(), headers);
        std::cout << "signup status response: " << response.getStatus() << std::endl;
        std::cout << "signup body response: " << response.getBody() << std::endl;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

int main()
{
    testSginUpHandler();
    testSginInHandler();
    return EXIT_SUCCESS;
}
