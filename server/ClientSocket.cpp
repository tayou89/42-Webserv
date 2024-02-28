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
  _tmp += tmp;

  /* header의 끝 찾기 */
  //   size_t pos = _header.find("\\r\\n\\r\\n");
  std::cout << "head\n";
  size_t pos = _tmp.find("  "); // test spliter
  if (pos != std::string::npos) {
    std::cout << pos << " " << readSize << " ~head\n";
    _header = _tmp.substr(0, pos);
    _body = _tmp.substr(pos + 2);
    _tmp.clear();
    // header에 대한 유효성 검사 추가할 부분
    _bodySize = _body.size(); // _req.getBodySize()로 수정할 예정
    if (readSize < 0) {
      _status = WRITE;
      return (1);
    }
    _status = BODYREAD; // body read 또는 Chunked 상태로 변경
  } else {
    // 바디가 없는 경우에 대해서 처리해줄 코드 만들어야됨!!!!!!!
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

  /* Body size check */
  //   if (readSize < BUFFERSIZE) { // temp option
  if (_body.size() >= _bodySize) {
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
