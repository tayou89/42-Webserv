#pragma once

#include "IEvent.hpp"
#include <string>

class IServer {
public:
  virtual ~IServer() {}
  virtual int acceptClient() = 0;
  virtual IEvent *eventRegister() = 0;
  virtual void getEvent(IEvent *event) = 0;
  virtual void deleteEvent(IEvent *event) = 0;
  virtual std::string recvEvent(int fd) = 0;
  virtual void sendEvent(int fd, std::string content) = 0;
  virtual void executeEvent() = 0;
};
