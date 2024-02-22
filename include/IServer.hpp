#pragma once

#include <string>

class IServer {
public:
  virtual ~IServer() {}
  virtual int getListenSocket() const = 0;
};
