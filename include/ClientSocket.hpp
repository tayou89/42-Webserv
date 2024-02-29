/* Save Current Status of Associated Event */

#pragma once

#include "IServer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "define.hpp"
#include <cstdlib>
#include <string>
#include <unistd.h>

class ClientSocket {

public:
  ClientSocket(int socket, IServer *acceptServer, char **envp);
  virtual ~ClientSocket();

  int readSocket();
  int readHead();
  int readContentBody();
  //   int readChunkedBody();
  int writeSocket();

private:
  ClientSocket();
  ClientSocket(const ClientSocket &ref);
  ClientSocket &operator=(const ClientSocket &ref);

  IServer *_routeServer;
  int _socket;
  int _status;
  char _buf[BUFFERSIZE + 1];
  std::string _tmp;
  std::string _header;
  size_t _bodySize;
  std::string _body;
  Request _req;
  Response _res;
  std::string _responseString;
};
