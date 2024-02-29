#pragma once

#include "Request.hpp"
#include "Response.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <unistd.h>

class Server {
public:
  Server();
  ~Server();

  void startServer(int fd);
  void setEnvp(char **envp);
  request getRequest() const;

private:
  Server(const Server &);
  Server &operator=(const Server &);
  // Protocol    _protocol;
  request _request;
  response _response;
  char **envp;
};
