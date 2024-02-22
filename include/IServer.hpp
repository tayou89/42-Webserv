#pragma once

#include "Protocol.hpp"
#include <string>

class IServer {
public:
  virtual ~IServer() {}
  virtual int getListenSocket() const = 0;
  virtual Protocol getProtocol() const = 0;
};
