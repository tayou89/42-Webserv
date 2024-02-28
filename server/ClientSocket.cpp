#include "../include/ClientSocket.hpp"

ClientSocket::ClientSocket() {}

ClientSocket::ClientSocket(const ClientSocket &ref) { static_cast<void>(ref); }

ClientSocket &ClientSocket::operator=(const ClientSocket &ref) {
  if (this == &ref)
    return (*this);

  return (*this);
}

ClientSocket::~ClientSocket() {}

/* essential part */

ClientSocket::ClientSocket(int socket, IServer *acceptServer)
    : _routeServer(acceptServer), _socket(socket) {
  this->_status = READABLE;
  memset(&_buf[0], 0, BUFFERSIZE + 1);
  _str.assign("");
  static_cast<void>(_routeServer); // remove after
}

int ClientSocket::readSocket() {
  if (_status != READABLE)
    return (0);

  memset(&_buf[0], 0, BUFFERSIZE + 1);
  size_t readSize = read(_socket, &_buf[0], BUFFERSIZE);
  if (readSize == 0)
    return (253);
  std::string tmp(_buf);
  _str += tmp;
  if (readSize < BUFFERSIZE) {
    _status = WRITEABLE;
    // send to protocol
    // _routeServer->getProtocol();
    // _req.setRequest(_str); // Requeset::setRequest(std::string &str);
    // _res.setResponse(_req);
    return (1);
  }
  return (0);
}

int ClientSocket::writeSocket() {
  if (_status != WRITEABLE)
    return (0);

  size_t writeSize = write(_socket, _str.c_str(), _str.size());
  if (writeSize != _str.size())
    return (254);
  _str.assign("");
  _status = READABLE;
  close(_responseFile);

  return (1);
}
