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
    : _routeServer(acceptServer), _socket(socket), _req(), _res() {
  this->_status = HEADREAD;
  memset(&_buf[0], 0, BUFFERSIZE + 1);
  _tmp.assign("");
  _header.assign("");
  _body.assign("");
  _bodySize = 0;
  _res.setEnvp(envp);
  static_cast<void>(_routeServer); // remove after
}

int ClientSocket::readHead() {
  memset(&_buf[0], 0, BUFFERSIZE + 1);
  _bodySize = 0;
  size_t readSize = read(_socket, &_buf[0], BUFFERSIZE);
  if (readSize == 0)
    return (253);
  std::string tmp(_buf);
  std::cout << tmp.size() << "\n";
  _tmp += tmp;
  for (std::string::iterator iter = _tmp.begin(); iter != _tmp.end(); iter++)
    std::cout << static_cast<int>(*iter) << " ";
  std::cout << "\n";

  /* header의 끝 찾기 */
  size_t pos = _tmp.find("\r\n\r\n");
  if (pos != std::string::npos) {
    _header = _tmp.substr(0, pos);
    _req.setRequest(_header);
    _status = _req.checkBodyExistence();
    if (_status == BODYREAD) {
      _body = _tmp.substr(pos + 4);
      _bodySize = atoi(_req.getRequestHeader("Content-length").c_str());
    }
    _tmp.clear();
    return (1); // 헤더의 상태에 따라서 달라짐
  }
  return (0);
}

int ClientSocket::readContentBody() { // chunked encoding는 별도의 함수로 처리
  memset(&_buf[0], 0, BUFFERSIZE + 1);
  size_t readSize = read(_socket, &_buf[0], BUFFERSIZE);
  if (readSize == 0)
    return (253);
  std::string tmp(_buf);
  _body += tmp;

  /* Body ssdfize check */
  //   if (_body.size() < _bodySize && readSize < BUFFERSIZE) {
  // 	// body size error
  // 	_req.getErrorResponse.create413Response()
  //   }
  if (_body.size() == _bodySize) {
    // make response instance
    _req.readBody(_body);
    _status = WRITE; // body read 상태로 변경
    return (1);
  }
  return (0);
}

int ClientSocket::readSocket() {
  if (_status == HEADREAD)
    return (readHead());
  else if (_status == BODYREAD)
    return (readContentBody());
  //   else if (_status == CHUNKED)
  //     return (readChunkedBody());
  return (0);
}

int ClientSocket::writeSocket() {
  if (_status != WRITE)
    return (0);

  if (_res.getResponseFile() == 0)
    _res.setResponse(_req);

  size_t writeSize =
      write(_socket, _res.getResponse().c_str(), _res.getResponse().size());
  // 버퍼사이즈 정하고 나눠서 쓰는걸로 바꿔야됨

  if (writeSize != _res.getResponse().size())
    return (254);
  close(_res.getResponseFile());

  return (1);
}
