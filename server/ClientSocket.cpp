#include "../include/ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(const ClientSocket &ref) { static_cast<void>(ref); }

ClientSocket &ClientSocket::operator=(const ClientSocket &ref) {
  if (this == &ref)
    return (*this);

  return (*this);
}

ClientSocket::~ClientSocket() {}

ClientSocket::ClientSocket(int socket, IServer *acceptServer)
    : _routeServer(acceptServer), _socket(socket) {
  this->_status = HEADREAD;
  memset(&_buf[0], 0, BUFFERSIZE + 1);
  _tmp.assign("");
  _header.assign("");
  _body.assign("");
  _bodySize = 0;
  _responseFile = 0;
  static_cast<void>(_routeServer); // remove after
}

int ClientSocket::readHead() {
  memset(&_buf[0], 0, BUFFERSIZE + 1);
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
    // check header
    _body = _tmp.substr(pos + 4); // 헤더 보고 바디 존재여부 판정
    _tmp.clear();
    _bodySize = _body.size(); // _req.getBodySize()로 수정
    _status = WRITE; // 헤더 확인 후 BODYREAD, CHUNKED, WRITE 판정
    return (1);      // 헤더의 상태에 따라서 달라짐
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
  //   if (readSize < BUFFERSIZE) { // temp option
  if (_body.size() == _bodySize) {
    // make response instance
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

  //   if (_responseFile == 0) {
  //     // _responseFile = _res.set(_req); // set response fd
  //     return (0);
  //   } else {
  //     // send response to client
  //   }
  size_t writeSize = write(_socket, _header.c_str(), _header.size());
  if (writeSize != _header.size())
    return (254);
  write(_socket, "\n", 1);
  writeSize = write(_socket, _body.c_str(), _body.size());
  if (writeSize != _body.size())
    return (254);
  _header.clear();
  _body.clear();
  _status = HEADREAD;
  close(_responseFile);
  _responseFile = 0;

  return (1);
}
