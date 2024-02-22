#pragma once

#include <fcntl.h>

class IServer;

class IEventLoop {
public:
  virtual ~IEventLoop() {}
  virtual void run() = 0;
};
