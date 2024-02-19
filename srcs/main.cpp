#include "../include/ClientStat.hpp"
#include "../include/EchoServer.hpp"
#include "../include/Kevent.hpp"
#include "../include/KqueueLoop.hpp"
// #include "../include/WebServer.hpp"
#include <iostream>

int main() {
  EchoServer test;
  std::map<int, IServer *> serverMap;

  serverMap[test.getListenSocket()] = &test;

  KqueueLoop eventLoop(serverMap);
  eventLoop.initServerSocket();
  eventLoop.run();
}
