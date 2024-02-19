#include "../include/Protocol.hpp"

Protocol::Protocol()
{
	this->_requestHeader.clear();
	this->_requestBody.clear();
	this->_requestMethod.clear();
	this->_requestURI.clear();
}

Protocol::~Protocol()
{

}

Protocol::Protocol(const Protocol&)
{

}

Protocol& Protocol::operator=(const Protocol&)
{
    return (*this);
}

std::string Protocol::readStartLine(std::string startLine, int large_client_header_buffers)
{
    std::string requestHTTPVersion;
    std::string::size_type pos1 = startLine.find(" ");
    std::string::size_type pos2 = startLine.find(" ", pos1 + 1);

	if (pos1 == std::string::npos || pos2 == std::string::npos)
		return (create400Response());
    this->_requestMethod = startLine.substr(0, pos1);
    this->_requestURI = startLine.substr(pos1 + 1, pos2 - pos1 - 1);
    requestHTTPVersion = startLine.substr(pos2 + 1, startLine.size() - pos2);

	//if it is not method sp URI sp HTTP-Version, create 400 response
	if (this->_requestMethod.size() == 0 || this->_requestURI.size() == 0 || requestHTTPVersion.size() == 0)
		return(create400Response());

	//if it is not GET, HEAD, DELETE, OPTIONS, POST, PUT, TRACE, create 400 response
	if (!(this->_requestMethod == "GET" || this->_requestMethod == "HEAD" || this->_requestMethod == "DELETE" || this->_requestMethod == "OPTIONS" || this->_requestMethod == "POST" || this->_requestMethod == "PUT" ||this->_requestMethod == "TRACE"))
		return(create400Response());

	//if the requestURI is longer than large_client_header_buffers, create 414 response
	if (this->_requestURI.size() > static_cast<unsigned long>(large_client_header_buffers))
		return(create414Response());

	//if the HTTP-Version is not HTTP/1.1, create 505 response, else create 400 response
	if (requestHTTPVersion != "HTTP/1.1")
	{
		if (requestHTTPVersion == "HTTP/0.9" || requestHTTPVersion == "HTTP/1.0" || requestHTTPVersion == "HTTP/2.0" || requestHTTPVersion == "HTTP/3.0")
			return(create505Response());
		else
			return(create400Response());
	}
	//if the requestFirstLine is valid, return a empty string
    return ("");
}

std::string Protocol::readHeader(std::string header)
{
	size_t pos = 0;
	size_t index = 0;
	// size_t colon_index = 0;
	std::string tmp;

	while (1)
	{
		index = header.find("\\r\\n", pos + 2);
		if (index == std::string::npos)
			break;
		tmp = header.substr(pos + 2, index - pos - 2);
		std::string key = splitBeforeColon(tmp);
		std::string value = splitAfterColon(tmp);
		this->_requestHeader.insert(std::make_pair(key, value));
		if (header.find("\\r\\n", index + 2) == index + 2)
			break;
		pos = index + 2;
	}
	for (std::map<std::string, std::string>::iterator it = this->_requestHeader.begin(); it != this->_requestHeader.end(); it++)
	{
		tmp = Protocol::checkValidHeader(it->first, it->second);
		if (tmp != "")
			return (tmp);
	}
	return ("");
}

std::string Protocol::checkValidHeader(std::string key, std::string value)
{
	if (key.empty() || value.empty())
		return (create400Response());

	//check validity of mandatory headers??

	return ("");
}

std::string Protocol::checkContentLength(int client_max_body_size)
{
	if (this->_requestHeader.find("Content-Length") != this->_requestHeader.end())
	{
		if (this->_requestHeader["Content-Length"].size() > 10)
			return (create400Response());
		if (std::stoi(this->_requestHeader["Content-Length"]) > client_max_body_size)
			return (create413Response());
	}
	return ("");
}

std::string Protocol::readBody(std::string body)
{
	// std::cout << "body is:" << body << std::endl;
	if (body.size() > static_cast<unsigned long>(std::stol(this->_requestHeader["Content-Length"])))
		return (create413Response());
	else if (body.size() < static_cast<unsigned long>(std::stol(this->_requestHeader["Content-Length"])))
		return (create400Response());
	else
		this->_requestBody = body;
	return ("");
}

std::string Protocol::getRequestMethod() const
{
	return (this->_requestMethod);
}

std::string Protocol::getRequestURI() const
{
	return (this->_requestURI);
}

std::string Protocol::getRequestBody() const
{
	return (this->_requestBody);
}

std::map<std::string, std::string> Protocol::getRequestHeader() const
{
	return (this->_requestHeader);
}

void Protocol::setRequestHeader(std::string key, std::string value)
{
	this->_requestHeader.insert(std::make_pair(key, value));
}

void Protocol::setResponseHeader(std::string key, std::string value)
{
	this->_responseHeader.insert(std::make_pair(key, value));
}

void Protocol::setResponseBody(std::string body)
{
	this->_responseBody = body;
}


////////////////////////////////////////////
//you MUST change the servername!!!!!!!!!!!!
////////////////////////////////////////////


std::string Protocol::create200Response()
{
	std::string response = "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() + "Server: " + "webserv\r\n";
	for (std::map<std::string, std::string>::iterator it = this->_responseHeader.begin(); it != this->_responseHeader.end(); it++)
		response += it->first + ": " + it->second + "\r\n";
	response = response + this->_responseBody;
	return (response);
}

std::string Protocol::create400Response()
{
	std::string response = "HTTP/1.1 400 Bad Request\r\n" + getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
	return (response);
}

std::string Protocol::create413Response()
{
	std::string response = "HTTP/1.1 413 Request Entity Too Large\r\n" + getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
	return (response);
}

std::string Protocol::create414Response()
{
	std::string response = "HTTP/1.1 414 URI Too Long\r\n" + getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
	return (response);
}

std::string Protocol::create505Response()
{
	std::string response = "HTTP/1.1 505 HTTP Version Not Supported\r\n" + getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
	return (response);
}