#ifndef INDIEPUB_SERVER_HPP
#define INDIEPUB_SERVER_HPP

#include <http/Server.hpp>
#include <csignal>
#include <memory>
#include <cstdlib>
#include <string>
#include <iostream>

class RESTfulAPI 
{
private:
    std::unique_ptr<HttpServer> apiServer;

    RESTfulAPI();

    void initEndpointHandlers();

public:
    static RESTfulAPI instance();
};

#endif // INDIEPUB_SERVER_HPP
