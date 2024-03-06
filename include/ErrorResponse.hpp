#pragma once

#include <iostream>
#include "../include/utils.hpp"
#include "Location.hpp"
#include "unistd.h"
#include "fcntl.h"
#include <map>

class errorResponse
{
    public:
        errorResponse();
        ~errorResponse();

        std::string     create200Response(Location loc, std::string serverName, std::map<std::string, std::string> responseHeader, std::string responseBody);
        std::string		create204Response(Location loc, std::string serverName);
		std::string		create400Response(Location loc, std::string serverName);
		std::string		create404Response(Location loc, std::string serverName);
		std::string		create405Response(Location loc, std::string serverName);
		std::string 	create413Response(Location loc, std::string serverName);
		std::string		create414Response(Location loc, std::string serverName);
		std::string		create500Response(Location loc, std::string serverName);
		std::string 	create505Response(Location loc, std::string serverName);
    private:
        errorResponse(const errorResponse& copy);
        errorResponse operator=(const errorResponse& copy);
};