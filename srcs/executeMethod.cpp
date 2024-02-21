#include "../include/TestServer.hpp"

void TestServer::GET_HEAD()
{
    int fd;

	char readbuf[10000];
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
	if (this->_protocol.getRequestMethod() == "GET")
		this->_protocol.setResponseBody(readbuf);
	this->_protocol.setResponseHeader("Content-Length", std::to_string(readSize));
	// this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
	this->_protocol.setResponseHeader("Content-Type", "text/html");
	this->_protocol.setResponseHeader("Content-Language", "en-US");
	this->_protocol.setResponseHeader("Last-Modified", getCurrentHttpDate());
}

void TestServer::POST()
{
    //straight to CGI (make two pipes, then fork)
	std::cout << "POST" << std::endl;
	//return 201 created
}

std::string getRmPath(char **envp)
{
	std::string path;

	while (*envp)
	{
		if (strncmp(*envp, "PATH\0", 3) == 0)
		{
			std::string tmp(*envp);
			path = tmp;
			break;
		}
		envp++;
	}
	while (path.size() > 0)
	{
		std::string RmPath = splitBeforeColon(path) + "/rm";
		std::cout << RmPath << std::endl;
		if (access(RmPath.c_str(), X_OK) != -1)
			return (RmPath);
		path = splitAfterColon(path);
	}
	std::cout << "cannot find rm" << std::endl;
	return ("");
}

// std::string	getOption()
// {

// }

void TestServer::DELETE()
{
	int fd;

	fd = open(this->_protocol.getRequestURI().c_str(), O_RDONLY);
	if (fd == -1)
	{
		//file does not exist, thus cannot be deleted
		std::cout << "open error" << std::endl;
		return ;
	}
	close(fd);
	std::string rmPath = getRmPath(envp);
	if (rmPath == "")
		return ;
	
	//leak warning
	char **option = (char **)malloc(sizeof(char *) * 3);
	option[0] = strdup("rm\0");
	option[1] = strdup(this->_protocol.getRequestURI().c_str());
	option[2] = NULL;
	execve(rmPath.c_str(), option, this->envp);
	free(option[0]);
	free(option[1]);
	free(option);
	//leak warning
	
	std::cout << "DELETE" << std::endl;
}

void TestServer::PUT()
{
	std::cout << "PUT" << std::endl;
}