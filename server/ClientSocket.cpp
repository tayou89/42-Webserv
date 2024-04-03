#include "../include/ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(const ClientSocket &ref) { static_cast<void>(ref); }

ClientSocket &ClientSocket::operator=(const ClientSocket &ref) {
  if (this == &ref)
    return (*this);

  return (*this);
}

ClientSocket::~ClientSocket() { delete _info; }

ClientSocket::ClientSocket(int socket, IServer *acceptServer, char **envp)
    : _routeServer(acceptServer), _req(acceptServer->getConfig()),
      _res(envp, acceptServer->getConfig()), _info(new struct eventInfo) {
  _socket = socket;
  this->_status = HEAD_READ;
  _buf.clear();
  _header.clear();
  _bodySize = 0;
  _processStatus = 0;
  _info->socket = _socket;
  _info->type = SOCKET;
}

std::string ClientSocket::getContentType() {
  std::string tmp(_cgiResponse.begin(), _cgiResponse.end());
  std::string header;
  std::size_t pos;

  if ((pos = tmp.find("\r\n\r\n")) != std::string::npos) {
    header = tmp.substr(0, pos);
    _cgiResponse.erase(_cgiResponse.begin(), _cgiResponse.begin() + pos + 4);
  } else if ((pos = tmp.find("\n\n")) != std::string::npos) {
    header = tmp.substr(0, pos);
    _cgiResponse.erase(_cgiResponse.begin(), _cgiResponse.begin() + pos + 2);
  }

  return (header);
}

std::string ClientSocket::getContentLength() {
  unsigned int length = _cgiResponse.size();
  if (length == 0)
    return (std::string(""));

  std::stringstream ss;

  ss << length;

  std::string contentLength = "Content-Length: " + ss.str();
  return (contentLength);
}

/* Process status 및 response 크기 보고 응답 상태 분류 */
struct eventStatus ClientSocket::sendCGIHeader() {
  if (_status != PIPE_TO_SOCKET_HEAD)
    return (makeStatus(CONTINUE, _socket));

  std::string type = getContentType();
  std::string head = "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() +
                     "Server: " + _routeServer->getConfig().getServerName() +
                     "\r\n" + getContentLength() + "\r\n";

  if (type.size() == 0) {
    head = head + "\r\n";
  } else {
    head = head + type + "\r\n\r\n";
  }

  std::size_t writeSize = write(_socket, &head[0], head.size());
  if (writeSize < 0) {
    return (makeStatus(DISCONNECT, _socket));
  }
  _status = PIPE_TO_SOCKET_BODY;
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::sendCGIBody() {
  if (_status != PIPE_TO_SOCKET_BODY)
    return (makeStatus(CONTINUE, _socket));

  int writeSize = write(_socket, &_cgiResponse[0], _cgiResponse.size());
  if (writeSize < 0) {
    return (makeStatus(DISCONNECT, _socket));
  }
  _cgiResponse.erase(_cgiResponse.begin(), _cgiResponse.begin() + writeSize);

  if (_cgiResponse.empty() == true) {
    _status = HEAD_READ;
    _info->socket = _socket;
    _info->type = SOCKET;
    return (makeStatus(SOCKET_READ_MODE, _socket));
  }

  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readPipe() {
  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  int readSize = read(_cgi.getReadFD(), &tmp[0], BUFFER_SIZE);
  _cgiResponse.insert(_cgiResponse.end(), tmp.begin(), tmp.begin() + readSize);
  if (readSize < BUFFER_SIZE) {
    _status = PIPE_TO_SOCKET_HEAD;
    close(_cgi.getReadFD());
    return (makeStatus(CGI_WRITE, _cgi.getReadFD()));
  }

  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::serverToPipe() {
  if (_status != SERVER_TO_PIPE_WRITE)
    return (makeStatus(CONTINUE, _socket));

  std::vector<unsigned char> body = _req.getRequestBody();

  int chunkSize = body.size();

  int writeSize = write(_cgi.getWriteFD(), &body[0], chunkSize);
  _req.eraseRequestBody(0, writeSize);
  if (_req.getRequestBody().size() == 0) {
    _status = PIPE_TO_SERVER;
    close(_cgi.getWriteFD());
    return (makeStatus(CGI_READ, _cgi.getReadFD(), _cgi.getWriteFD()));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::processFinish() {
  waitpid(_cgi.getPID(), &_processStatus, 0);
  if (WEXITSTATUS(_processStatus) != 0) {
    close(_cgi.getReadFD());
    _status = WRITE;
    _info->type = SOCKET;
    throw(_errorResponse.create500Response(_routeServer->getConfig()));
  }
  if (_status == CGI_TO_PIPE)
    _status = PIPE_TO_SERVER;

  return (makeStatus(CGI_READ, _cgi.getReadFD()));
}

struct eventStatus ClientSocket::eventProcess(struct kevent *event, int type) {
  struct eventStatus result;

  if (type == SOCKET) {
    if (_status == PIPE_TO_SOCKET_HEAD)
      result = sendCGIHeader();
    else if (_status == PIPE_TO_SOCKET_BODY)
      result = sendCGIBody();
    else if (event->filter == EVFILT_READ &&
             static_cast<int>(event->ident) == _socket) {
      result = readSocket();
    } else if (event->filter == EVFILT_WRITE &&
               static_cast<int>(event->ident) == _socket) {
      result = writeSocket();
    }
  } else if (type == PIPE) {
    if (event->filter == EVFILT_PROC) {
      try {
        result = processFinish();
      } catch (std::string &res) {
        _responseString = res;
        return (makeStatus(SOCKET_WRITE_MODE, _socket));
      }
    } else if (event->filter == EVFILT_READ &&
               static_cast<int>(event->ident) == _cgi.getReadFD()) {
      result = readPipe();
    } else if (event->filter == EVFILT_WRITE) {
      if (_status == SERVER_TO_PIPE_WRITE &&
          static_cast<int>(event->ident) != _socket) {
        result = serverToPipe();
      } else if (_status == PIPE_TO_SOCKET_HEAD &&
                 static_cast<int>(event->ident) == _socket) {
        try {
          result = sendCGIHeader();
        } catch (std::string &res) {
          _responseString = res;
        }
      } else if (_status == PIPE_TO_SOCKET_BODY &&
                 static_cast<int>(event->ident)) {
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

struct eventInfo *ClientSocket::getEventInfo() { return (_info); }

struct eventStatus ClientSocket::readHead() {
  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  int readSize = read(_socket, &tmp[0], BUFFER_SIZE);
  if (readSize == 0) {
    clearSocket();
    return (makeStatus(DISCONNECT, _socket));
  } else if (readSize == -1)
    return (makeStatus(DISCONNECT, _socket));

  _buf.insert(_buf.end(), tmp.begin(), tmp.begin() + readSize);
  std::vector<unsigned char>::iterator iter = findHeader(_buf);

  if (iter != _buf.end()) {
    _header = std::string(_buf.begin(), iter + 2);
    _buf.erase(_buf.begin(), iter + 4);

    try {
      _req.setRequest(_header);
    } catch (std::string &res) {
      _responseString = res;
    }
    _status = _req.checkBodyExistence();
    if (_status == BODY_READ) {
      _bodySize = std::atoi(_req.getRequestHeader("Content-Length").c_str());
      if (_buf.size() >= _bodySize) {
        _status = WRITE;
        _body.insert(_body.end(), _buf.begin(), _buf.end());
        _buf.clear();
        _req.readBody(_body);
        return (makeStatus(SOCKET_WRITE_MODE, _socket));
      }
      return (makeStatus(CONTINUE, _socket));
    } else if (_status == CHUNKED_READ) {
      return (readChunkedBody());
    } else
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readContentBody() {
  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  if (!_buf.empty()) {
    _body.insert(_body.end(), _buf.begin(), _buf.end());
    _buf.clear();
  }

  int readSize = read(_socket, &tmp[0], BUFFER_SIZE);
  if (readSize == 0) {
    _req.readBody(_body);
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  } else if (readSize == -1) {
    return (makeStatus(DISCONNECT, _socket));
  }

  _body.insert(_body.end(), tmp.begin(), tmp.begin() + readSize);

  if (_body.size() == _bodySize) {
    _req.readBody(_body);
    _status = WRITE;
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readChunkedBody() {
  std::vector<unsigned char> buf(BUFFER_SIZE, 0);

  int readSize = read(_socket, &buf[0], BUFFER_SIZE);
  if (readSize > 0) {
    _buf.insert(_buf.end(), buf.begin(), buf.begin() + readSize);
    if (_bodySize != 0 && _buf.size() > _bodySize + 2) {
      _body.insert(_body.end(), _buf.begin(), _buf.begin() + _bodySize);
      _buf.erase(_buf.begin(), _buf.begin() + _bodySize + 2);
      _bodySize = 0;
    } else if (_bodySize != 0)
      return (makeStatus(CONTINUE, _socket));
  } else if (_buf.size() == 0) {
    _status = WRITE;
    _req.readBody(_body);
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  } else if (readSize == -1)
    return (makeStatus(DISCONNECT, _socket));

  std::string str(_buf.begin(), _buf.end());
  while (str.size()) {
    std::string sizeStr = str.substr(0, str.find("\r\n"));
    std::istringstream iss(sizeStr);
    iss >> std::hex >> _bodySize;

    if (_bodySize == 0) {
      _buf.clear();
      _req.readBody(_body);
      _status = WRITE;
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
    }
    str.erase(0, sizeStr.size() + 2);

    std::string chunk;

    if (str.size() > _bodySize) {
      chunk = str.substr(0, _bodySize);
      str.erase(0, _bodySize + 2);
      _bodySize = 0;
      _body.insert(_body.end(), chunk.begin(), chunk.end());
    } else {
      _bodySize -= str.size();
      _body.insert(_body.end(), str.begin(), str.end());
      str.clear();
    }
  }
  _buf.clear();
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

  if (_responseString.empty()) {
    try {
      _req.checkRequestValidity();
      _req.convertURI();
      _req.checkRequestMethod();
      if (_req.getLocation().getCGIPass()) {
        _status = _cgi.setCGIExecutor(_req);
        _info->type = PIPE;
        return (_cgi.execute());
      } else {
        _res.setResponse(_req);
        _responseString = _res.getResponse();
        return (makeStatus(CONTINUE, _socket));
      }
    } catch (std::string &res) {
      _responseString = res;
      return (makeStatus(CONTINUE, _socket));
    }
  }

  int writeSize =
      write(_socket, _responseString.c_str(), _responseString.size());
  if (writeSize < 0) {
    return (makeStatus(DISCONNECT, _socket));
  }
  _responseString.erase(0, writeSize);
  if (_responseString.empty()) {
    if (_req.getRequestHeader("Connection") == "close") {
      clearSocket();
      return (makeStatus(DISCONNECT, _socket));
    }
    return (makeStatus(SOCKET_READ_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

void ClientSocket::clearSocket() {
  _status = HEAD_READ;
  _buf.clear();
  _header.clear();
  _body.clear();
  _bodySize = 0;
  _responseString.clear();
  _req.initRequest();
  _res.initResponse();
  _info->socket = _socket;
  _info->type = SOCKET;
  _cgi = CGIExecutor();
  _cgiResponse.clear();
  _processStatus = 0;
}
