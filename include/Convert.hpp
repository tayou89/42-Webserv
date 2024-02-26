#pragma once

#include "Protocol.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

class Convert {
public:
  Convert();
  Convert(const Convert &);
  ~Convert();

  void getRequest(int fd);
  void checkValidity();
  void executeMethod();
  void sendResponse(int fd);

  void setEnvp(char **envp);
  std::string getPath(char **envp, std::string cmd);

  void GET_HEAD();
  void POST();
  void DELETE();
  void PUT();
  void OPTIONS();
  void TRACE();

private:
  Convert &operator=(const Convert &);

  Protocol _protocol;
  char **envp;
};
