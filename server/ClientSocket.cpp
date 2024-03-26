#include "../include/ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(const ClientSocket &ref) { static_cast<void>(ref); }

ClientSocket &ClientSocket::operator=(const ClientSocket &ref) {
  if (this == &ref)
    return (*this);

  return (*this);
}

ClientSocket::~ClientSocket() {}

ClientSocket::ClientSocket(int socket, IServer *acceptServer, char **envp)
    : _routeServer(acceptServer), _req(acceptServer->getConfig()),
      _res(envp, acceptServer->getConfig()) {
  _socket = socket;
  this->_status = HEAD_READ;
  _buf.clear();
  _header.clear();
  _bodySize = 0;
  _info.socket = _socket;
  _info.type = SOCKET;
}

std::string ClientSocket::getContentType() {
  std::string tmp(_cgiResponse.begin(), _cgiResponse.end());
  std::string header;
  size_t pos;

  if ((pos = tmp.find("\r\n\r\n")) != std::string::npos) {
    header = tmp.substr(0, pos);
    _cgiResponse.erase(_cgiResponse.begin(), _cgiResponse.begin() + pos + 4);
  } else if ((pos = tmp.find("\n\n")) != std::string::npos) {
    header = tmp.substr(0, pos);
    _cgiResponse.erase(_cgiResponse.begin(), _cgiResponse.begin() + pos + 2);
  }

  return (header);
}

struct eventStatus ClientSocket::sendCGIHeader() {
  if (_cgiResponse.empty() == true)
    return (makeStatus(CONTINUE, _socket));

  std::string head = "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() +
                     "Server: " + _routeServer->getConfig().getServerName() +
                     "\r\n" + getContentType() + "\r\n" +
                     "Transfer-Encoding: Chunked\r\n\r\n";

  size_t writeSize = write(_socket, &head[0], head.size());
  if (writeSize != head.size())
    ; // write error
  _status = PIPE_TO_SOCKET_BODY;
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::sendCGIBody() {
  if (_processStatus == END && _cgiResponse.empty() == true) {
    write(_socket, "0\r\n\r\n", 5);
    _status = HEAD_READ;
    _info.type = SOCKET;
    return (makeStatus(SOCKET_READ_MODE, _socket, _cgi.getWriteFD()));
  } else if (_cgiResponse.empty() == true)
    return (makeStatus(CONTINUE, _socket));

  int chunkSize = _cgiResponse.size();
  std::stringstream ss;

  ss << std::hex << chunkSize;

  std::string sizeStr = ss.str() + "\r\n";

  size_t sizeWrite = write(_socket, sizeStr.c_str(), sizeStr.size());
  if (sizeWrite != sizeStr.size())
    ; // write error
  size_t chunkWrite = write(_socket, &_cgiResponse[0], _cgiResponse.size());
  if (chunkWrite != _cgiResponse.size())
    ; // write error
  _cgiResponse.clear();
  write(_socket, "\r\n", 2);

  if (_processStatus == END) {
    write(_socket, "0\r\n\r\n", 5);
    _status = HEAD_READ;
    _info.type = SOCKET;
    return (makeStatus(SOCKET_READ_MODE, _socket, _cgi.getWriteFD()));
  }

  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readPipe() {
  if (_processStatus == END) {
    return (makeStatus(CONTINUE, _socket));
  }

  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  int readSize = read(_cgi.getReadFD(), &tmp[0], BUFFER_SIZE);
  if (readSize == -1) {
    return (makeStatus(CONTINUE, _socket));
  }

  _cgiResponse.insert(_cgiResponse.end(), tmp.begin(), tmp.begin() + readSize);

  int pid = waitpid(_cgi.getPID(), &_processStatus, WNOHANG);
  if (pid != 0) {
    close(_cgi.getReadFD());
    _processStatus = END;
    return (makeStatus(CGI_READ_END, _cgi.getReadFD()));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::socketToPipe() {
  if (_status != SOCKET_TO_PIPE_WRITE)
    return (makeStatus(CONTINUE, _socket));
  std::cout << "socket to pipe start\n";

  std::vector<unsigned char> body = _req.getRequestBody();

  int writeSize = write(_cgi.getWriteFD(), &body[0], body.size());
  std::cout << writeSize << std::endl;
  _req.eraseRequestBody(0, writeSize);
  //   if (writeSize == -1)
  //   throw _res.getErrorResponse().create403Response();
  if (_req.getRequestBody().size() == 0) {
    std::cout << "socket to pipe finish\n";
    close(_cgi.getWriteFD());
    _status = PIPE_TO_SOCKET_HEAD;
    return (makeStatus(CGI_READ, _cgi.getReadFD(), _cgi.getWriteFD()));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::eventProcess(struct kevent *event, int type) {
  struct eventStatus result;

  if (type == SOCKET) {
    if (event->filter == EVFILT_READ) {
      // SOCKET READ
      result = readSocket();
    } else if (event->filter == EVFILT_WRITE) {
      result = writeSocket();
    }
  } else if (type == PIPE) {
    if (event->filter == EVFILT_READ) {
      result = readPipe();
    } else if (event->filter == EVFILT_WRITE) {
      if (_status == SOCKET_TO_PIPE_WRITE) {
        // request body의 내용을 PIPE로 전달할 때
        result = socketToPipe();
      } else if (_status == PIPE_TO_SOCKET_HEAD) {
        // status PIPE HEAD 처음 버퍼의 내용을 소켓에 작성할 때
        try {
          result = sendCGIHeader();
        } catch (std::string &res) {
          _responseString = res;
        }
      } else if (_status == PIPE_TO_SOCKET_BODY) {
        // status PIPE BODY 그 이후에 버퍼의 내용을 소켓에 작성할 때
        try {
          result = sendCGIBody();
        } catch (std::string &res) {
          _responseString = res;
        }
      }
    }
  }
  return (result);
}

struct eventInfo &ClientSocket::getEventInfo() { return (_info); }

struct eventStatus ClientSocket::readHead() {
  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  size_t readSize = read(_socket, &tmp[0], BUFFER_SIZE);
  if (readSize == 0) {
    clearSocket();
    return (makeStatus(DISCONNECT, _socket));
  }

  _buf.insert(_buf.end(), tmp.begin(), tmp.begin() + readSize);

  std::string tmpStr(_buf.begin(), _buf.end());

  /* header의 끝 찾기 */
  size_t pos = tmpStr.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _header = tmpStr.substr(0, pos + 2);
    _buf.erase(_buf.begin(), _buf.begin() + pos + 4);
    try {
      _req.setRequest(_header);
    } catch (std::string &res) {
      _responseString = res;
      _status = WRITE;
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
    }
    _status = _req.checkBodyExistence();
    std::cout << "status: " << _status << std::endl;
    if (_status == BODY_READ) { // read normal body
      _bodySize = atoi(_req.getRequestHeader("Content-Length").c_str());
      std::cout << "body size: " << _bodySize << std::endl;
      std::cout << "buffer size: " << _buf.size() << std::endl;
      if (_buf.size() >= _bodySize) {
        _status = WRITE;
        _req.readBody(_buf);
        return (makeStatus(SOCKET_WRITE_MODE, _socket));
      }
      return (makeStatus(CONTINUE, _socket));
    } else if (_status == CHUNKED_READ) { // read chunked body
      return (makeStatus(CONTINUE, _socket));
    } else
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readContentBody() {
  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  size_t readSize = read(_socket, &tmp[0], BUFFER_SIZE);
  if (readSize == 0) {
    clearSocket();
    return (makeStatus(DISCONNECT, _socket));
  }

  _buf.insert(_buf.end(), tmp.begin(), tmp.begin() + readSize);

  /* Body size check */
  //   if (_body.size() < _bodySize && readSize < BUFFER_SIZE) {
  //     // body size error
  //     // _req.getErrorResponse.create413Response();
  //   }
  if (_buf.size() == _bodySize) {
    // make response instance
    try {
      _req.readBody(_buf);
    } catch (std::string &res) {
      _responseString = res;
    }
    _status = WRITE; // write 상태로 변경
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

/* \r\n을 이용해서 한줄씩 읽는 방식으로 변경해야됨 */
struct eventStatus ClientSocket::readChunkedBody() {
  //   std::string tmp;
  //   std::string raw;
  //   std::ostringstream oss;
  //   size_t chunkSize;
  //   int readSize = BUFFER_SIZE;

  //   while (readSize == BUFFER_SIZE) {
  //     memset(&_buf, 0, BUFFER_SIZE);
  //     readSize = read(_socket, &_buf, BUFFER_SIZE);
  //     if (readSize < 0) {
  //       // read error 처리 추가
  //     }
  //     tmp.append(_buf, readSize);
  //     raw += tmp;
  //   }

  //   memset(&_buf, 0, BUFFER_SIZE);

  //   std::istringstream iss(raw);

  //   while (!iss.eof()) {
  //     std::string chunk;

  //     iss >> std::hex >> chunkSize;
  //     iss.ignore(2);

  //     if (chunkSize == 0) {
  //       _status = WRITE;
  //       return (makeStatus(SOCKET_WRITE_MODE, _socket));
  //     }
  //     chunk.resize(chunkSize);
  //     iss.read(&chunk[0], chunkSize);
  //     iss.ignore(2);

  //     _body += chunk;
  //   }

  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readSocket() {
  if (_status == HEAD_READ)
    return (readHead());
  else if (_status == BODY_READ)
    return (readContentBody());
  else if (_status == CHUNKED_READ)
    return (readChunkedBody());
  else
    return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::writeSocket() {
  if (_status != WRITE)
    return (makeStatus(CONTINUE, _socket));

  if (_responseString.size() == 0) {
    try {
      std::cout << "this is URI1:" << _req.getRequestURI() << std::endl;
      _req.convertURI();
      std::cout << "this is URI2:" << _req.getRequestURI() << std::endl;
      if (_req.getLocation().getCGIPass()) {
        _status = _cgi.setCGIExecutor(_req);
        _processStatus = ALIVE;
        _info.type = PIPE;
        return (_cgi.execute());
      } else {
        _res.setResponse(_req);
        _responseString = _res.getResponse();
      }
    } catch (std::string &res) {
      _responseString = res;
      return (makeStatus(CONTINUE, _socket));
    }
  }

  size_t writeSize =
      write(_socket, _responseString.c_str(), _responseString.size());

  if (writeSize != _responseString.size())
    ; //   return (WRITE_ERROR);

  if (_req.getRequestHeader("Connection") == "close") {
    clearSocket();
    return (makeStatus(DISCONNECT, _socket));
  }

  return (makeStatus(SOCKET_READ_MODE, _socket));
}

void ClientSocket::clearSocket() {
  _status = HEAD_READ;
  _buf.clear();
  _header.clear();
  _bodySize = 0;
  _responseString.clear();
  _req.initRequest();
  _res.initResponse();
  _processStatus = END;
}
