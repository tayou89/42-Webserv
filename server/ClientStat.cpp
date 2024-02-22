#include "../include/ClientStat.hpp"

ClientStat::ClientStat() {}

ClientStat::ClientStat(const ClientStat &ref) { static_cast<void>(ref); }

ClientStat &ClientStat::operator=(const ClientStat &ref) {
  if (this == &ref)
    return (*this);

  return (*this);
}

ClientStat::~ClientStat() {}

/* essential part */

ClientStat::ClientStat(int socket, IServer *acceptServer)
    : _routeServer(acceptServer), _socket(socket) {
  this->_status = READABLE;
  memset(&_buf[0], 0, BUFFERSIZE);
  _str.assign("");
  static_cast<void>(_routeServer); // remove after
}

int ClientStat::readSocket() {
  if (_status != READABLE)
    return (0);

  memset(&_buf[0], 0, BUFFERSIZE);
  size_t readSize = read(_socket, &_buf[0], BUFFERSIZE);
  if (readSize == 0)
    return (253);
  std::string tmp(_buf);
  _str += tmp;
  if (readSize < BUFFERSIZE)
    _status = WRITEABLE;
  return (0);
}

int ClientStat::writeSocket() {
  if (_status != WRITEABLE)
    return (0);

  size_t writeSize = write(_socket, _str.c_str(), _str.size());
  if (writeSize != _str.size())
    return (254);
  _str.assign("");
  _status = READABLE;
  return (0);
}
