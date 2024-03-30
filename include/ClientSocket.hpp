/* Save Current Status of Associated Event */

#pragma once

#include "CGIExecutor.hpp"
#include "ErrorResponse.hpp"
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
  struct eventInfo *getEventInfo();
  struct eventStatus readSocket();
  struct eventStatus readHead();
  struct eventStatus readContentBody();
  struct eventStatus readChunkedBody();
  struct eventStatus writeSocket();
  struct eventStatus processFinish();
  struct eventStatus serverToPipe();
  struct eventStatus sendCGIHeader();
  struct eventStatus sendCGIBody();
  struct eventStatus readPipe();
  std::string getContentType();
  std::string getContentLength();
  void clearSocket();

private:
  ClientSocket();
  ClientSocket(const ClientSocket &ref);
  ClientSocket &operator=(const ClientSocket &ref);

  IServer *_routeServer;
  int _socket;
  int _status;
  std::vector<unsigned char> _buf;
  std::string _header;
  std::vector<unsigned char> _body;
  std::size_t _bodySize;
  Request _req;
  Response _res;
  std::string _responseString;
  struct eventInfo *_info;
  CGIExecutor _cgi;
  std::vector<unsigned char> _cgiResponse;
  int _processStatus;
  ErrorResponse _errorResponse;
};
