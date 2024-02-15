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

ClientStat::ClientStat(IServer *acceptServer) : _routeServer(acceptServer) {
  this->_status = INITSTATUS;
}
