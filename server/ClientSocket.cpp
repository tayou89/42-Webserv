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
    : _routeServer(acceptServer), _socket(socket),
      _req(_routeServer->getConfig()), _res(envp, _routeServer->getConfig()) {
  this->_status = HEAD_READ;
  memset(&_buf[0], 0, BUFFER_SIZE + 1);
  _tmp.assign("");
  _header.assign("");
  _body.assign("");
  _bodySize = 0;
  _info.socket = _socket;
  _info.type = SOCKET;
}

std::string ClientSocket::makeCGIresponse(std::string result) const {
  size_t end = result.find("\r\n\r\n");
  std::string body = result.substr(end + 4);

  std::string head = "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() +
                     "Server: " + _routeServer->getConfig().getServerName() +
                     "\r\n";

  size_t contentLength = body.size();

  std::stringstream ss;
  ss << contentLength;
  head += "Content-Length: " + ss.str() + "\r\n";

  return (head + result);
}

void ClientSocket::binaryResponse(std::vector<char> vec) const {
  std::vector<char>::iterator it = vec.begin();
  int idx = 0;

  while (it != vec.end()) {
    if (*it == '\r' && *(it + 1) == '\n' && *(it + 2) == '\r' &&
        *(it + 3) == '\n')
      break;
    it++;
    idx++;
  }
  idx += 4;

  it = vec.begin();
  std::string head = "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() +
                     "Server: " + _routeServer->getConfig().getServerName() +
                     "\r\n";

  size_t contentLength = vec.size() - idx;

  std::stringstream ss;
  ss << contentLength;
  head += "Content-Length: " + ss.str() + "\r\n";

  write(_socket, &head[0], head.size());
  while (it != vec.end()) {
    write(_socket, &(*it), 1);
    it++;
  }
}

struct eventStatus ClientSocket::eventProcess(struct kevent *event, int type) {
  struct eventStatus result;

  if (type == SOCKET) {
    if (event->filter == EVFILT_READ) {
      // SOCKET READ
      result = readSocket();
    } else if (event->filter == EVFILT_WRITE) {
      // SOCKET WRITE
      result = writeSocket();
    }
  } else if (type == PIPE) {
    if (event->filter == EVFILT_PROC) {
      std::vector<char> bufVec;
      std::vector<char> tmp(BUFFER_SIZE + 1);
      int readSize = BUFFER_SIZE + 1;

      waitpid(_cgi.getPID(), NULL, 0);
      while (readSize > 0) {
        memset(&tmp[0], 0, BUFFER_SIZE + 1);
        readSize = read(_cgi.getReadFD(), &tmp[0], BUFFER_SIZE + 1);
        bufVec.insert(bufVec.end(), tmp.begin(), tmp.begin() + readSize);
      }
      binaryResponse(bufVec);
      close(_cgi.getReadFD());
      _info.type = SOCKET;
      _status = HEAD_READ;
      return (makeStatus(SOCKET_READ_MODE, _socket));
    }
  }
  return (result);
}

struct eventInfo &ClientSocket::getEventInfo() { return (_info); }

struct eventStatus ClientSocket::readHead() {
  memset(&_buf[0], 0, BUFFER_SIZE + 1);
  _bodySize = 0;
  size_t readSize = read(_socket, &_buf[0], BUFFER_SIZE);
  if (readSize == 0)
    return (makeStatus(DISCONNECT, _socket));

  std::string tmp(_buf);
  _tmp += tmp;

  /* header의 끝 찾기 */
  size_t pos = _tmp.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _header = _tmp.substr(0, pos + 2);
    try {
      _req.setRequest(_header);
    } catch (std::string &res) {
      _responseString = res;
      _status = WRITE;
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
    }
    _status = _req.checkBodyExistence();
    if (_status == BODY_READ) { // read normal body
      _body = _tmp.substr(pos + 4);
      _bodySize = atoi(_req.getRequestHeader("Content-length").c_str());
      _tmp.clear();
      return (makeStatus(CONTINUE, _socket));
    } else if (_status == CHUNKED_READ) { // read chunked body
      _tmp.clear();
      return (makeStatus(CONTINUE, _socket));
    } else
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readContentBody() {
  memset(&_buf[0], 0, BUFFER_SIZE + 1);
  size_t readSize = read(_socket, &_buf[0], BUFFER_SIZE);
  std::string tmp(_buf);
  _body += tmp;

  /* Body size check */
  if (_body.size() < _bodySize && readSize < BUFFER_SIZE) {
    // body size error
    // _req.getErrorResponse.create413Response();
  }
  if (_body.size() == _bodySize) {
    // make response instance
    try {
      _req.readBody(_body);
    } catch (std::string &res) {
      _responseString = res;
    }
    _status = WRITE; // write 상태로 변경
    return (makeStatus(SOCKET_WRITE_MODE, _socket));
  }
  return (makeStatus(CONTINUE, _socket));
}

struct eventStatus ClientSocket::readChunkedBody() {
  std::string tmp;
  std::string raw;
  std::ostringstream oss;
  size_t chunkSize;
  int readSize = BUFFER_SIZE;

  while (readSize == BUFFER_SIZE) {
    memset(&_buf, 0, BUFFER_SIZE);
    readSize = read(_socket, &_buf, BUFFER_SIZE);
    if (readSize < 0) {
      // read error 처리 추가
    }
    tmp.append(_buf, readSize);
    raw += tmp;
  }

  memset(&_buf, 0, BUFFER_SIZE);

  std::istringstream iss(raw);

  while (!iss.eof()) {
    std::string chunk;

    iss >> std::hex >> chunkSize;
    iss.ignore(2);

    if (chunkSize == 0) {
      _status = WRITE;
      return (makeStatus(SOCKET_WRITE_MODE, _socket));
    }
    chunk.resize(chunkSize);
    iss.read(&chunk[0], chunkSize);
    iss.ignore(2);

    _body += chunk;
  }

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
      if (_req.getLocation().getCGIPass()) {
        _status = PIPE_WRITE;
        _info.type = PIPE;
        _cgi.setCGIExecutor(_req);
        _cgi.createPipeFD();
        _cgi.createProcess();
        _cgi.setPipeFD();
        if (_cgi.getPID() > 0) {
          return (makeStatus(PROCESS, _cgi.getPID()));
        }
        _cgi.executeCGI(); // child process execve
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
    return (makeStatus(DISCONNECT, _socket));
  }

  return (makeStatus(SOCKET_READ_MODE, _socket));
}

void ClientSocket::clearSocket() {
  _status = HEAD_READ;
  memset(&_buf, 0, BUFFER_SIZE + 1);
  _tmp.clear();
  _header.clear();
  _bodySize = 0;
  _body.clear();
  _responseString.clear();
  _req = Request();
  _res.initResponse();
}
