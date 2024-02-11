#pragma once

#include <iostream>
#include <map>

class Protocol {
    public :
		Protocol();
        ~Protocol();
		std::string	readStartLine(std::string startLine, int large_client_header_buffers);
		std::string readHeader(std::string header);
		std::string checkValidHeader(std::string key, std::string value);
		std::string checkContentLength(int client_max_body_size);
		std::string readBody(std::string body);
		std::string create400Response();
		std::string create413Response();
		std::string create414Response();
		std::string create505Response();

    private :
        Protocol(const Protocol&);
        Protocol& operator=(const Protocol&);
        std::map<std::string, std::string> _requestHeader;
		std::string _requestBody;
		std::string _requestMethod;
		std::string _requestURI;
    
};