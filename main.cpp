#include "./include/ClientSocket.hpp"
#include "./include/ConfigMain.hpp"
#include "./include/KqueueLoop.hpp"
#include "./include/WebServer.hpp"
#include <csignal>
#include <cstdlib>
#include <iostream>

std::map<int, IServer *> serverMap;

void signalHandler(int signal) {
  std::map<int, IServer *>::iterator iter = serverMap.begin();
  for (; iter != serverMap.end(); iter++) {
    close(iter->second->getListenSocket());
    delete iter->second;
  }

  exit(signal);
}

int main(int argc, char *argv[], char *envp[]) {
  if (argc > 2)
    exit(1);

  ConfigMain configMain(argv[1]);

  std::vector<Config> confVec = configMain.getServerConfigs();
  std::vector<Config>::iterator iter = confVec.begin();

  signal(SIGINT, signalHandler);
  signal(SIGQUIT, signalHandler);
  signal(SIGPIPE, SIG_IGN);

  for (; iter != confVec.end(); iter++) {
    WebServer *temp = new WebServer(*iter);
    serverMap[temp->getListenSocket()] = temp;
  }

  KqueueLoop eventLoop(serverMap, envp);
  eventLoop.initServerSocket();
  eventLoop.run();
}
