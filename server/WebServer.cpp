#include "../include/WebServer.hpp"
#include <iostream> // test

WebServer::WebServer(Config conf) { // + protocol
  _listenSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (_listenSocket == -1)
    exit(1); // socket error

  memset(&_serverAddress, 0, sizeof(_serverAddress));
  _serverAddress.sin_family = AF_INET;
  //   _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  _serverAddress.sin_port = htons(conf.getPortNumber());
  inet_pton(AF_INET, conf.getIPAddress().c_str(),
            &_serverAddress.sin_addr.s_addr);

  if (bind(_listenSocket, (struct sockaddr *)&_serverAddress,
           sizeof(_serverAddress)) == -1) {
    std::cout << "bind error\n";
    exit(1); // bind error
  }

  if (listen(_listenSocket, 5) == -1)
    exit(1); // listen error
  fcntl(_listenSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

WebServer::~WebServer() {}

int WebServer::getListenSocket() const { return (_listenSocket); }
