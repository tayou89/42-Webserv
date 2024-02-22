#pragma once

#include "IServer.hpp"
#include <arpa/inet.h>
#include <map>
#include <sys/event.h>
#include <sys/socket.h>

class WebServer : public IServer {
public:
  WebServer(Config conf, Protocol protocol); // bind, listen
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

  // listenSocket을 반환하는 함수 필요함.

private:
  WebServer();
  WebServer(const WebServer &);
  WebServer &operator=(const WebServer &);

  int _listenSocket; // set on constructor
  struct sockaddr_in _serverAddress;
  std::map<int, std::string> _clientSocket;

  Config _conf;
  Protocol _protocol;
};

/* Server의 Configuration의 일부를 Server에서 내부적으로 가지고 있는것이
 * 좋을지에 대한 고찰 -> 필요하다면 parsing이 완료된 후 constructor에서 처리 */

// ex) std::map<std::string, Location> _location = Config.getLocation();
