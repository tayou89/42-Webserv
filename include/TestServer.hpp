#pragma once

#include "Protocol.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

class TestServer {
public:
  TestServer();
  ~TestServer();

  void getRequest(int fd);
  void checkValidity();
  void executeMethod();
  void sendResponse(int fd);

  void setEnvp(char **envp);

  void GET_HEAD();
  void POST();
  void DELETE();
  void PUT();
  void OPTIONS();
  void TRACE();

private:
  TestServer(const TestServer &);
  TestServer &operator=(const TestServer &);

  Protocol _protocol;
  char **envp;
};
