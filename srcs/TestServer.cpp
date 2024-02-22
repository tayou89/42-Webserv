#include "../include/Protocol.hpp"
#include "../include/TestServer.hpp"

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

void TestServer::setEnvp(char **envp)
{
	this->envp = envp;
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

	//check if \r\n\r\n and \r\n exists
    if (packet.find("\\r\\n\\r\\n") == std::string::npos || packet.find("\\r\\n") == std::string::npos)
    {
		//create400Response();
		std::cout << packet << std::endl;
		std::cout << "create400Response" << std::endl;
		return ;
	}
	std::string firstLine = packet.substr(0, packet.find("\\r\\n"));
    firstLine = this->_protocol.readStartLine(firstLine, LARGE_CLIENT_HEADER_BUFFERS);
	if (firstLine != "")
	{
		std::cout << firstLine;
		exit(1);
	}

	std::string header = packet.substr(packet.find("\\r\\n") + 2, packet.find("\\r\\n\\r\\n") - packet.find("\\r\\n") - 2);
	header = this->_protocol.readHeader(header);
	if (header != "")
		exit(1);
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
		// 1. check index directive in conf file
		// 2. check if autoindex is enabled
		// 3. check if try_files exist
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
	if (this->_protocol.getRequestMethod() == "GET" || this->_protocol.getRequestMethod() == "HEAD")
		GET_HEAD();
	else if (this->_protocol.getRequestMethod() == "POST")
		POST();
	else if (this->_protocol.getRequestMethod() == "DELETE")
		DELETE();
	else if (this->_protocol.getRequestMethod() == "PUT")
		PUT();
	else if (this->_protocol.getRequestMethod() == "OPTIONS")
		OPTIONS();
	else if (this->_protocol.getRequestMethod() == "TRACE")
		TRACE();
}

void TestServer::sendResponse(int fd)
{
	(void)fd;
	//print instead of sending packet
	std::cout << this->_protocol.getResponse() << std::endl;
}