#include "./include/ClientStat.hpp"
#include "./include/ConfigMain.hpp"
#include "./include/KqueueLoop.hpp"
#include "./include/WebServer.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
  if (argc > 2)
    exit(1); // too many arguments error
  ConfigMain configMain(argv[1]);
  std::map<int, IServer *> serverMap;

  std::vector<Config> confVec = configMain.getServerConfigs();
  std::vector<Config>::iterator iter = confVec.begin();

  for (; iter != confVec.end(); iter++) {
    WebServer temp(*iter);
    serverMap[temp.getListenSocket()] = &temp;
  }

  KqueueLoop eventLoop(serverMap);
  eventLoop.initServerSocket();
  eventLoop.run();
}
