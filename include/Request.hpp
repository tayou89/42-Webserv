#pragma once

#include <iostream>
#include <map>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "ErrorResponse.hpp"

class request
{
    public :
        request();
        ~request();
        request operator=(const request &copy);
        request(const request &copy);
        
        void    setRequest(std::string packet);
        // void	checkValidity();
        void	readStartLine(std::string startLine, int large_client_header_buffers);
		void	readHeader(std::string header);
		void	checkValidHeader(std::string key, std::string value);
		void	checkContentLength(int client_max_body_size);
		void	readBody(std::string body);

        std::string getRequestMethod() const;
		std::string getRequestURI() const;
		std::string getRequestBody() const;
		std::map<std::string, std::string> getRequestHeader() const;
		std::string getRequestHeader(std::string key) const;
		void		setRequestHeader(std::string key, std::string value);
    private :
        std::string _request;
        std::map<std::string, std::string> _requestHeader;
		std::string _requestBody;
		std::string _requestMethod;
		std::string _requestURI;

        errorResponse _errorResponse;
};