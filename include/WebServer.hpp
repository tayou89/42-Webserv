#pragma once

#include "IServer.hpp"
#include <sys/event.h>
#include <vector>

class WebServer : public IServer {
public:
  WebServer(Config conf, Protocol protocol);
  virtual ~WebServer();

  // accept new client
  int acceptClient();

  // ev.newEvent(ev.sv[idx].eventRegister())
  IEvent *eventRegister();

  //_WebServerList[idx].getEvent(eventlist[j]);
  void getEvent(IEvent *event);

  // read socket descriptor when new event occurred, call protocol
  std::string recvEvent(int fd);

  // register new send Event. send content to fd.
  void sendEvent(int fd, std::string content);

  // input something returned from protocol (instance or std::string or ...)
  void excuteEvent();

private:
  WebServer();
  WebServer(const WebServer &);
  WebServer &operator=(const WebServer &);

  int _listenSocket; // set on constructor
  std::vector<int> _fds;

  Config _conf;
  Protocol _protocol;
};
