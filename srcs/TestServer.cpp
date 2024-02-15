#include "../include/Protocol.hpp"
#include "../include/TestServer.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

#define LARGE_CLIENT_HEADER_BUFFERS 1000
#define MAX_BODY_SIZE 1000

TestServer::TestServer()
{

}
        
TestServer::~TestServer()
{

}

TestServer::TestServer(const TestServer&)
{

}

TestServer& TestServer::operator=(const TestServer&)
{
	return (*this);
}

void TestServer::getRequest(int fd)
{
	char buf[LARGE_CLIENT_HEADER_BUFFERS + MAX_BODY_SIZE + 1];
    int readSize = read(fd, buf, LARGE_CLIENT_HEADER_BUFFERS + MAX_BODY_SIZE);
    if (readSize == -1)
    {
        std::cout << "read error" << std::endl;
        exit(1);
    }
    buf[readSize] = '\0';
    std::string packet(buf);
    
    //check if \r\n\r\n exists
    if (packet.find("\\r\\n\\r\\n") == std::string::npos)
    {
		//create400Response();
		std::cout << packet << std::endl;
		std::cout << "create400Response" << std::endl;
		return ;
	}
	std::string firstLine = packet.substr(0, packet.find("\\r\\n"));
    this->_protocol.readStartLine(firstLine, LARGE_CLIENT_HEADER_BUFFERS);
	std::string header = packet.substr(packet.find("\\r\\n") + 2, packet.find("\\r\\n\\r\\n") - packet.find("\\r\\n") - 2);
	this->_protocol.readHeader(header);

	// if (chunked == true)
	// 	std::string body = packet.substr(packet.find("\r\n\r\n") + 4, getcontentLength());
	// else
	// {
	// 	//read more chunked data
	// }

    //for now, let's say that the body is well added and ready to execute.
    std::string body = packet.substr(packet.find("\\r\\n\\r\\n") + 8);
	std::map<std::string, std::string> pro = this->_protocol.getRequestHeader();
	if (pro.find("Content-Length") != pro.end())
	{
		// std::cout << "content-length is:" << pro["Content-Length"] << std::endl;
		std::string tmpBody = this->_protocol.readBody(body);
		if (tmpBody != "")
			std::cout << tmpBody << std::endl;
	}
}

void TestServer::checkValidity()
{
	DIR *dir = opendir(this->_protocol.getRequestURI().c_str());
	if (dir != NULL)
	{ //if URI is a directory
		// std::cout << "directory" << std::endl;
		closedir(dir);
	}
	else
	{ //if URI is a file
		// std::cout << "file" << std::endl;
	}

	//check more...
}

void TestServer::executeMethod()
{
	char readbuf[10000];
	if (this->_protocol.getRequestMethod() == "GET")
	{
		int fd;

		fd = open(this->_protocol.getRequestURI().c_str(), O_RDONLY);
		if (fd == -1)
		{
			std::cout << "open error" << std::endl;
			return ;
		}
		int readSize = read(fd, readbuf, 10000);
		if (readSize == -1)
		{
			std::cout << "read error" << std::endl;
			return ;
		}
		readbuf[readSize] = '\0';
		this->_protocol.setResponseBody(readbuf);
		this->_protocol.setResponseHeader("Content-Length", std::to_string(readSize));
		// this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
		this->_protocol.setResponseHeader("Content-Type", "text/html");
		this->_protocol.setResponseHeader("Content-Language", "en-US");
		this->_protocol.setResponseHeader("Last-Modified", getCurrentHttpDate());
	}
	else if (this->_protocol.getRequestMethod() == "POST")
	{
		std::cout << "POST" << std::endl;
	}
	else if (this->_protocol.getRequestMethod() == "DELETE")
	{
		std::cout << "DELETE" << std::endl;
	}
	else if (this->_protocol.getRequestMethod() == "PUT")
	{
		std::cout << "PUT" << std::endl;
	}
	else if (this->_protocol.getRequestMethod() == "HEAD")
	{
		std::cout << "HEAD" << std::endl;
	}
}

void TestServer::sendResponse(int fd)
{
	(void)fd;
	std::string response = this->_protocol.create200Response();
	std::cout << response << std::endl;
}