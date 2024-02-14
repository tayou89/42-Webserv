#pragma once

#include "IEventLoop.hpp"
#include "IServer.hpp"
#include "Kevent.hpp"
#include <fcntl.h>
#include <map>
#include <sys/event.h>
#include <vector>

class KqueueLoop : public IEventLoop {
public:
  KqueueLoop(std::map<int, IServer *> serverList);
  virtual ~KqueueLoop();
  void addEvent(Kevent event);
  //   void eventAlert(IServer *); // 아마 필요 없을듯?
  void initServerSocket(); // make listening socket and register at _changeList
  void run();

private:
  KqueueLoop();
  KqueueLoop(const KqueueLoop &);
  KqueueLoop &operator=(const KqueueLoop &);

  int _kqueue;
  std::vector<struct kevent> _changeList;

  /* _eventList에 server instance를 저장하면 필요없어질수도? */
  std::map<int, IServer *> _serverList;

  std::map<int, IEvent *> _eventList; // IEvent class refactor required
};
