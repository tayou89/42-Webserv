/* Save Current Status of Associated Event */

#pragma once

#include "CGIExecutor.hpp"
#include "IServer.hpp"
#include "Request.hpp"
#include "Response.hpp"
#include "define.hpp"
#include "struct.hpp"
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class ClientSocket {

public:
  ClientSocket(int socket, IServer *acceptServer, char **envp);
  virtual ~ClientSocket();

  struct eventStatus eventProcess(struct kevent *event, int type);
  struct eventInfo &getEventInfo();
  struct eventStatus readSocket();
  struct eventStatus readHead();
  struct eventStatus readContentBody();
  struct eventStatus readChunkedBody();
  struct eventStatus writeSocket();
  struct eventStatus sendCGIHeader();
  struct eventStatus sendCGIBody();
  struct eventStatus readPipe();
  std::string getContentType();
  //   std::string makeCGIresponse(std::string result) const;
  //   void binaryResponse(std::vector<char> vec) const;
  void clearSocket();

private:
  ClientSocket();
  ClientSocket(const ClientSocket &ref);
  ClientSocket &operator=(const ClientSocket &ref);

  IServer *_routeServer;
  int _socket;
  int _status;
  char _buf[BUFFER_SIZE + 1];
  std::string _tmp;
  std::string _header;
  size_t _bodySize;
  std::string _body;
  Request _req;
  Response _res;
  std::string _responseString;
  struct eventInfo _info;
  CGIExecutor _cgi;
  std::vector<unsigned char> _cgiResponse;
  int _processStatus;
};
