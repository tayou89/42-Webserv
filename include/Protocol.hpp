#pragma once

#include <iostream>
#include <map>

#include "utils.hpp"

class Protocol {
    public :
		Protocol();
        ~Protocol();

		void	readStartLine(std::string startLine, int large_client_header_buffers);
		void	readHeader(std::string header);
		void	checkValidHeader(std::string key, std::string value);
		void	checkContentLength(int client_max_body_size);
		void	readBody(std::string body);
		std::string		create200Response();
		std::string		create204Response();
		std::string		create400Response();
		std::string		create404Response();
		std::string 	create413Response();
		std::string		create414Response();
		std::string		create500Response();
		std::string 	create505Response();

		std::string getRequestMethod() const;
		std::string getRequestURI() const;
		std::string getRequestBody() const;
		std::map<std::string, std::string> getRequestHeader() const;
		void		setRequestHeader(std::string key, std::string value);

		void		setResponseHeader(std::string key, std::string value);
		void		setResponseBody(std::string body);
		void		setResponse(std::string response);
		std::string	getResponse();

    private :
        Protocol(const Protocol&);
        Protocol& operator=(const Protocol&);
        std::map<std::string, std::string> _requestHeader;
		std::string _requestBody;
		std::string _requestMethod;
		std::string _requestURI;

		std::map<std::string, std::string> _responseHeader;
		std::string _responseBody;
		std::string _response;
    
};