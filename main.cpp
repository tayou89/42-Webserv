#include "./include/ClientStat.hpp"
#include "./include/ConfigMain.hpp"
#include "./include/KqueueLoop.hpp"
#include "./include/Protocol.hpp"
#include "./include/WebServer.hpp"
#include <iostream>

// int main(int argc, char *argv[], char *envp[]) {
//   if (argc > 2)
//     exit(1); // too many arguments error

//   ConfigMain configMain(argv[1]);
//   Protocol protocol;
//   std::map<int, IServer *> serverMap;

//   std::vector<Config> confVec = configMain.getServerConfigs();
//   std::vector<Config>::iterator iter = confVec.begin();

//   for (; iter != confVec.end(); iter++) {
//     WebServer temp(*iter, protocol, envp);
//     serverMap[temp.getListenSocket()] = &temp;
//   }

//   KqueueLoop eventLoop(serverMap);
//   eventLoop.initServerSocket();
//   eventLoop.run();
// }
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

int main(void)
{
    if (access("/Users/tayou/Desktop/tayou/temp", F_OK) == 0)
        std::cout << "File is existed\n";
    else
        std::cout << "File is not existed\n";
    if (access("/Users/tayou/Desktop/tayou/temp/aaa", R_OK) == 0)
        std::cout << "File is readable\n";
    else
        std::cout << "File is not readable\n";
    if (access("/Users/tayou/Desktop/tayou/temp/aaa", X_OK) == 0)
        std::cout << "File is executable\n";
    else
        std::cout << "File is not executable\n";
    return (0);
}