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
    if (event->filter == EVFILT_WRITE) {
      // PIPE WRITE
    } else if (event->filter == EVFILT_READ) {
      // PIPE READ
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
      std::cout << res << std::endl;
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
      //   std::cout << "this is URI 1:" << _req.getRequestURI() << std::endl;
      _req.convertURI();
      if (_req.getLocation().getCGIPass()) {
        std::cout << "cgi\n";
        // cgi execute -> get pipe fd
        _status = PIPE_WRITE;
        // return (makeStatus(PIPE_WRITE_MODE, pipefd));
      } else {
        _res.setResponse(_req);
        _responseString = _res.getResponse();
        //   std::cout << "this is method:" << _req.getRequestMethod() <<
        //   std::endl; std::cout << "this is URI 2:" << _req.getRequestURI() <<
        //   std::endl;
      }
    } catch (std::string &res) {
      _responseString = res;
      return (makeStatus(CONTINUE, _socket));
    }
  }

  //   std::cout << "///////////////////////////////////////////\n";
  //   std::cout << _responseString;
  //   std::cout << _socket << " : SOCKET NUMBER\n";
  //   std::cout << "///////////////////END/////////////////////\n";

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
