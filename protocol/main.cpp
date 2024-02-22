#include "../include/Protocol.hpp"
#include "../include/TestServer.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv, char **envp) {
  Protocol protocol;
  TestServer testserver;
  int fd;

  (void)argc;
  (void)argv;
  fd = open("test.txt", O_RDONLY);
  if (fd == -1) {
    std::cout << "open error" << std::endl;
    return 1;
  }
  testserver.setEnvp(envp);
  testserver.getRequest(fd);
  testserver.checkValidity();
  testserver.executeMethod();
  testserver.sendResponse(fd);
  return 0;
}
