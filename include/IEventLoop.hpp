#pragma once

#include <fcntl.h>

class IServer;
class IEvent;

class IEventLoop {
public:
  virtual ~IEventLoop() {}
  virtual void newEvent(const IEvent *) = 0;
  virtual void alert(IServer *) = 0;
  virtual void run() = 0;
};
