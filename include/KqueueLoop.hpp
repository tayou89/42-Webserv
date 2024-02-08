#pragma once

#include "IEvent.hpp"
#include "IEventLoop.hpp"
#include "Server.hpp"
#include <fcntl.h>
#include <map>

class KqueueLoop : public IEventLoop {
public:
  KqueueLoop();
  virtual ~KqueueLoop();
  void addEvent(IEvent *event);
  void alert(IServer *);
  void run();

private:
  KqueueLoop(const KqueueLoop &);
  KqueueLoop &operator=(const KqueueLoop &);

  int _kqueue;
  std::vector<IEvent *> _changeList;
  std::map<int, IServer *> _serverList; // _serverList[EVENT]
};
