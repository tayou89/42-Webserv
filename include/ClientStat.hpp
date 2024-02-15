/* Save Current Status of Associated Event */

#pragma once

#include "IServer.hpp"

#define INITSTATUS 0

class ClientStat {

public:
  ClientStat(IServer *acceptServer);
  virtual ~ClientStat();

private:
  ClientStat();
  ClientStat(const ClientStat &ref);
  ClientStat &operator=(const ClientStat &ref);

  IServer *_routeServer;
  int _status;
};
