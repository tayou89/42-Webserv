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
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <dirent.h>

int	main(void)
{
	DIR				*directory = opendir("/Users/tayou/Downloads");
	struct dirent	*entry;

	if (directory == NULL)
		std::cout << "Opening directory failed\n";	
	entry = readdir(directory);
	while (entry != NULL)
	{
		std::cout << entry->d_name << '\n';
		entry = readdir(directory);
	}
	return (0);
}