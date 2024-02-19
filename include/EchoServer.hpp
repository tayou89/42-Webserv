#pragma once

#include "IServer.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <sys/socket.h>

class EchoServer : public IServer {

public:
  EchoServer();
  ~EchoServer();

  int getListenSocket() const;

private:
  int _listenSocket;
  struct sockaddr_in _serverAddress;
  std::map<int, std::string> _clientSocket;
};
