/* Save Current Status of Associated Event */

#pragma once

#include "IServer.hpp"
#include <unistd.h>

#define READABLE 0
#define WRITEABLE 1
#define BUFFERSIZE 1024

class ClientStat {

public:
  ClientStat(int socket, IServer *acceptServer);
  virtual ~ClientStat();

  int readSocket();
  int writeSocket();

private:
  ClientStat();
  ClientStat(const ClientStat &ref);
  ClientStat &operator=(const ClientStat &ref);

  IServer *_routeServer;
  int _socket;
  int _status;
  char _buf[BUFFERSIZE];
  std::string _str;
};
