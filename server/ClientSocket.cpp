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
  if (_processStatus == CLOSEPIPE && _cgiResponse.empty() == true) {
    write(_socket, "0\r\n\r\n", 5);
    _status = HEAD_READ;
    _info.type = SOCKET;
    return (makeStatus(SOCKET_READ_MODE, _socket, _cgi.getWriteFD()));
  } else if (_cgiResponse.empty() == true)
    return (makeStatus(CONTINUE, _socket));

  int chunkSize = _cgiResponse.size();
  //   std::cout << "Response size: " << chunkSize << std::endl;
  if (chunkSize > 1000)
    chunkSize = 1000;

  //   std::cout << "chunk size: " << chunkSize << std::endl;

  std::stringstream ss;

  ss << std::hex << chunkSize;

  std::string sizeStr = ss.str() + "\r\n";

  size_t sizeWrite = write(_socket, sizeStr.c_str(), sizeStr.size());
  if (sizeWrite != sizeStr.size())
    ;
  size_t chunkWrite = write(_socket, &_cgiResponse[0], chunkSize);
  if (chunkWrite < 1)
    return (makeStatus(CONTINUE, _socket)); // write error
  write(_socket, "\r\n", 2);
  _cgiResponse.erase(_cgiResponse.begin(), _cgiResponse.begin() + chunkWrite);

  //   if (_processStatus == END) {
  //     write(_socket, "0\r\n\r\n", 5);
  //     _status = HEAD_READ;
  //     _info.type = SOCKET;
  //     return (makeStatus(SOCKET_READ_MODE, _socket, _cgi.getWriteFD()));
  //   }

  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readPipe() {
  std::vector<unsigned char> tmp(BUFFER_SIZE, 0);

  int readSize = read(_cgi.getReadFD(), &tmp[0], BUFFER_SIZE);
  if (readSize == -1) {
    return (makeStatus(CONTINUE, _socket));
  } else if (_processStatus == END && readSize == 0) {
    _processStatus = CLOSEPIPE;
    close(_cgi.getReadFD());
    return (makeStatus(CGI_READ_END, _cgi.getReadFD()));
  }

  _cgiResponse.insert(_cgiResponse.end(), tmp.begin(), tmp.begin() + readSize);

  int pid = waitpid(_cgi.getPID(), &_processStatus, WNOHANG);
  if (pid != 0)
    _processStatus = END;

  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::socketToPipe() {
  if (_status != SOCKET_TO_PIPE_WRITE)
    return (makeStatus(CONTINUE, _socket));

  std::vector<unsigned char> body = _req.getRequestBody();

  int writeSize = write(_cgi.getWriteFD(), &body[0], body.size());
  //   std::cout << "Write size: " << writeSize << std::endl;
  //   std::cout << "Remain body size:" << body.size() << std::endl;
  _req.eraseRequestBody(0, writeSize);
  //   if (writeSize == -1) {
  //     perror("Error");
  //   }
  if (_req.getRequestBody().size() == 0) {
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
  std::vector<unsigned char>::iterator iter = findHeader(_buf);

  if (iter != _buf.end()) {
    _header = std::string(_buf.begin(), iter + 2);
    _buf.erase(_buf.begin(), iter + 4);

    std::cout << "this is request:\n" << _header << "\n------------------------" << std::endl;
    std::cout << std::string(_buf.begin(), _buf.end()) << std::endl;

    _req.setRequest(_header);
    _status = _req.checkBodyExistence();
    if (_status == BODY_READ) { // read normal body
      _bodySize = atoi(_req.getRequestHeader("Content-Length").c_str());
      if (_buf.size() >= _bodySize) {
        _status = WRITE;
        _body.insert(_body.end(), _buf.begin(), _buf.end());
        _buf.clear();
        _req.readBody(_body);
        return (makeStatus(SOCKET_WRITE_MODE, _socket));
      }
      return (makeStatus(CONTINUE, _socket));
    } else if (_status == CHUNKED_START) { // read chunked body
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

  size_t readSize = read(_socket, &tmp[0], BUFFER_SIZE);
  if (readSize == 0) {
    _req.readBody(_body);
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }

  _body.insert(_body.end(), tmp.begin(), tmp.begin() + readSize);

  if (_body.size() == _bodySize) {
    _req.readBody(_body);
    _status = WRITE; // write 상태로 변경
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

/* \r\n을 이용해서 한줄씩 읽는 방식으로 변경해야됨 */
struct eventStatus ClientSocket::readChunkedBody() {
  std::vector<unsigned char> buf(BUFFER_SIZE, 0);

  if (_status != CHUNKED_START && _buf.empty()) {
    int readSize = read(_socket, &buf[0], BUFFER_SIZE);
    _buf.insert(_buf.end(), buf.begin(), buf.begin() + readSize);
    if (readSize != 0) {
      if (_bodySize != 0 && _buf.size() > _bodySize) {
        _body.insert(_body.end(), buf.begin(), buf.begin() + _bodySize);
        _buf.erase(buf.begin(), buf.begin() + _bodySize + 2);
        _bodySize = 0;
      }
    }
  }

  std::string str(_buf.begin(), _buf.end());
  while (str.size()) {

    std::string sizeStr = str.substr(0, str.find("\r\n"));
    std::istringstream iss(sizeStr);
    iss >> std::hex >> _bodySize;

    std::cout << "chunked: " << _bodySize << std::endl;
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
  _status = CHUNKED_READ;
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
      _req.convertURI();
        std::cout << "this is body:\n";
      _req.checkRequestValidity();
      if (_req.getLocation().getCGIPass()) {
        _status = _cgi.setCGIExecutor(_req);
        _processStatus = ALIVE;
        _info.type = PIPE;
        return (_cgi.execute());
      } else {
        _res.setResponse(_req);
        _responseString = _res.getResponse();
        std::cout << "this is response:\n" << _responseString << std::endl;
      }
    } catch (std::string &res) {
      _responseString = res;
        std::cout << "this is response:\n" << res << std::endl;
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
  _body.clear();
  _bodySize = 0;
  _responseString.clear();
  _req.initRequest();
  _res.initResponse();
  _processStatus = END;
}
