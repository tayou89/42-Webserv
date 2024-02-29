#include "./include/ClientSocket.hpp"
#include "./include/ConfigMain.hpp"
#include "./include/KqueueLoop.hpp"
#include "./include/WebServer.hpp"
#include <iostream>

int main(int argc, char *argv[], char *envp[]) {
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

  KqueueLoop eventLoop(serverMap, envp);
  eventLoop.initServerSocket();
  eventLoop.run();
}
