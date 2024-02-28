/* Save Current Status of Associated Event */

#pragma once

#include "IServer.hpp"
#include <unistd.h>

#define READABLE 0
#define WRITEABLE 1
#define BUFFERSIZE 1023

class ClientSocket {

public:
  ClientSocket(int socket, IServer *acceptServer);
  virtual ~ClientSocket();

  int readSocket();
  int writeSocket();

private:
  ClientSocket();
  ClientSocket(const ClientSocket &ref);
  ClientSocket &operator=(const ClientSocket &ref);

  IServer *_routeServer;
  int _socket;
  int _status;
  char _buf[BUFFERSIZE + 1];
  std::string _str;
  //   Request _req;
  //   Response _res;
  int _responseFile;
};
