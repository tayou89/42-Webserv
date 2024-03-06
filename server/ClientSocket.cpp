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
    : _routeServer(acceptServer), _socket(socket), _req(),
      _res(envp, _routeServer->getConfig()) {
  this->_status = HEAD_READ;
  memset(&_buf[0], 0, BUFFER_SIZE + 1);
  _tmp.assign("");
  _header.assign("");
  _body.assign("");
  _bodySize = 0;
}

int ClientSocket::readHead() {
  memset(&_buf[0], 0, BUFFER_SIZE + 1);
  _bodySize = 0;
  size_t readSize = read(_socket, &_buf[0], BUFFER_SIZE);
  if (readSize == 0)
    return (DISCONNECT);
  std::string tmp(_buf);
  std::cout << tmp.size() << "\n";
  _tmp += tmp;
  //   for (std::string::iterator iter = _tmp.begin(); iter != _tmp.end();
  //   iter++)
  //     std::cout << static_cast<int>(*iter) << " ";
  //   std::cout << "\n";

  /* header의 끝 찾기 */
  size_t pos = _tmp.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _header = _tmp.substr(0, pos);
    std::cout << _header << std::endl;
    try {
      _req.setRequest(_header);
    } catch (std::string &res) {
      std::cout << "Head read error\n";
      _responseString = res;
      _status = WRITE;
      return (WRITE_MODE);
    }
    _status = _req.checkBodyExistence();
    if (_status == BODY_READ) { // read normal body
      _body = _tmp.substr(pos + 4);
      _bodySize = atoi(_req.getRequestHeader("Content-length").c_str());
      _tmp.clear();
      return (CONTINUE);
    } else if (_status == CHUNKED_READ) { // read chunked body
      _tmp.clear();
      return (CONTINUE);
    } else                 // body not existence
      return (WRITE_MODE); // 헤더의 상태에 따라서 달라짐
  }
  return (0);
}

int ClientSocket::readContentBody() { // chunked encoding는 별도의 함수로 처리
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
      std::cout << "Body read error\n";
      _responseString = res;
    }
    _status = WRITE; // write 상태로 변경
    return (WRITE_MODE);
  }
  return (CONTINUE);
}

int ClientSocket::readChunkedBody() {
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
      std::cout << _body << "\n";
      _status = WRITE;
      return (WRITE_MODE);
    }
    chunk.resize(chunkSize);
    iss.read(&chunk[0], chunkSize);
    iss.ignore(2);

    _body += chunk;
  }

  return (CONTINUE);
}

int ClientSocket::readSocket() {
  if (_status == HEAD_READ)
    return (readHead());
  else if (_status == BODY_READ)
    return (readContentBody());
  else if (_status == CHUNKED_READ)
    return (readChunkedBody());
  return (CONTINUE);
}

int ClientSocket::writeSocket() {
  if (_status != WRITE)
    return (CONTINUE);

  if (_responseString.size() == 0) {
    try {
      _res.setResponse(_req);
      _responseString = _res.getResponse();
    } catch (std::string &res) {
      std::cout << "Write error\n";
      _responseString = res;
      return (CONTINUE);
    }
  }

  size_t writeSize =
      write(_socket, _responseString.c_str(), _responseString.size());

  if (writeSize != _responseString.size())
    return (WRITE_ERROR);
  close(_res.getResponseFile());

  return (READ_MODE);
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
  //   _res = Response();
}
