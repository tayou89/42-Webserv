#include "../include/EchoServer.hpp"
#include <iostream> // test

#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

EchoServer::EchoServer() {
  _listenSocket = socket(PF_INET, SOCK_STREAM, 0);

  if (_listenSocket == -1)
    exit(1); // socket error

  memset(&_serverAddress, 0, sizeof(_serverAddress));
  _serverAddress.sin_family = AF_INET;
  _serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  _serverAddress.sin_port = htons(8080);

  if (bind(_listenSocket, (struct sockaddr *)&_serverAddress,
           sizeof(_serverAddress)) == -1) {
    std::cout << "bind error\n";
    exit(1); // bind error
  }

  if (listen(_listenSocket, 5) == -1)
    exit(1); // listen error
  fcntl(_listenSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

EchoServer::~EchoServer() {}

int EchoServer::getListenSocket() const { return (_listenSocket); }
