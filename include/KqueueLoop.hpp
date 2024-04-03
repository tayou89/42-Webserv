#pragma once

#include "ClientSocket.hpp"
#include "IEventLoop.hpp"
#include "IServer.hpp"
#include "define.hpp"
#include "struct.hpp"
#include <fcntl.h>
#include <map>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class ClientStat;

class KqueueLoop : public IEventLoop {
public:
  KqueueLoop(std::map<int, IServer *> serverList, char **envp);
  virtual ~KqueueLoop();
  void initServerSocket();
  void newEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                uint32_t fflags, intptr_t data, void *udata);
  void eventHandler(struct kevent *event);
  void disconnect(int socket);
  void run();

private:
  KqueueLoop();
  KqueueLoop(const KqueueLoop &);
  KqueueLoop &operator=(const KqueueLoop &);

  int _kqueue;
  std::vector<struct kevent> _changeList;
  std::map<int, IServer *> _serverList;
  char **_envp;

  std::map<int, ClientSocket *> _clientList;
};
