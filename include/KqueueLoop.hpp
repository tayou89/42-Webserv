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
  void alert(IServer *);
  void initServerSocket(); // make listening socket and register at _changeList
  void run();

private:
  KqueueLoop();
  KqueueLoop(const KqueueLoop &);
  KqueueLoop &operator=(const KqueueLoop &);

  int _kqueue;
  std::vector<struct kevent> _changeList;
  std::map<int, IServer *> _serverList; // _serverList[EVENT]
};
