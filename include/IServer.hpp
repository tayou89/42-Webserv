#pragma once

#include "Config.hpp"
#include <string>

class IServer {
public:
  virtual ~IServer() {}
  virtual int getListenSocket() const = 0;
  virtual Config &getConfig() = 0;
};
