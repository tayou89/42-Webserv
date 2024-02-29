#pragma once

#include <iostream>
#include "../include/utils.hpp"
#include <map>

class errorResponse
{
    public:
        errorResponse();
        ~errorResponse();

        std::string     create200Response(std::map<std::string, std::string> responseHeader, std::string responseBody);
        std::string		create204Response();
		std::string		create400Response();
		std::string		create404Response();
		std::string		create405Response();
		std::string 	create413Response();
		std::string		create414Response();
		std::string		create500Response();
		std::string 	create505Response();
    private:
        errorResponse(const errorResponse& copy);
        errorResponse operator=(const errorResponse& copy);
};