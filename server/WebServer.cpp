#include "../include/WebServer.hpp"
#include <iostream>

WebServer::WebServer(Config conf) : _config(conf) {
  _listenSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (_listenSocket == -1)
    exit(1); // socket error

  /* set socket option */
  int optval = 1;
  if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) < 0) {
    close(_listenSocket);
    exit(1);
  }

  memset(&_serverAddress, 0, sizeof(_serverAddress));
  _serverAddress.sin_family = AF_INET;
  _serverAddress.sin_port = htons(conf.getPortNumber());
  inet_pton(AF_INET, conf.getIPAddress().c_str(),
            &_serverAddress.sin_addr.s_addr);

  if (bind(_listenSocket, (struct sockaddr *)&_serverAddress,
           sizeof(_serverAddress)) == -1) {
    std::cout << "bind error\n";
    exit(1);
  }

  if (listen(_listenSocket, 64) == -1)
    exit(1);
  fcntl(_listenSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

WebServer::~WebServer() {}

int WebServer::getListenSocket() const { return (_listenSocket); }

Config &WebServer::getConfig() { return (_config); }
