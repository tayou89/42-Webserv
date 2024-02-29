#include "../include/Server.hpp"

#define LARGE_CLIENT_HEADER_BUFFERS 1000
#define MAX_BODY_SIZE 1000

Server::Server() {}

Server::~Server() {}

Server::Server(const Server &) {}

Server &Server::operator=(const Server &) { return (*this); }

void Server::setEnvp(char **envp) { this->envp = envp; }

void Server::startServer(int fd) {
  char buf[LARGE_CLIENT_HEADER_BUFFERS + MAX_BODY_SIZE + 1];
  int readSize = read(fd, buf, LARGE_CLIENT_HEADER_BUFFERS + MAX_BODY_SIZE);
  if (readSize == -1) {
    std::cout << "read error" << std::endl;
  }
  buf[readSize] = '\0';
  std::string packet(buf);

  try {
    this->_request.setRequest(packet);
    this->_response.setEnvp(this->envp);
    request parRequest(this->getRequest());
    std::string response = this->_response.setResponse(parRequest);
    std::cout << response << std::endl;
  } catch (std::string &e) {
    std::cerr << e << std::endl; // it is the error string, so just put it in a
                                 // packet and send it
  }
}

request Server::getRequest() const { return (this->_request); }

// void Server::sendResponse(int fd)
// {
// 	(void)fd;
// 	//print instead of sending packet
// 	std::cout << this->_protocol.getResponse() << std::endl;
// }
