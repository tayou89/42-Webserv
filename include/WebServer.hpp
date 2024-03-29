#pragma once

#include "Config.hpp"
#include "IServer.hpp"
#include <arpa/inet.h>
#include <fcntl.h>
#include <map>
#include <sys/event.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

class WebServer : public IServer {

public:
  WebServer(Config conf);
  ~WebServer();

  int getListenSocket() const;
  Config &getConfig();

private:
  Config _config;
  int _listenSocket;
  struct sockaddr_in _serverAddress;
};
