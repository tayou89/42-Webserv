#pragma once

#include "ClientStat.hpp"
#include "IEventLoop.hpp"
#include "IServer.hpp"
#include <fcntl.h>
#include <map>
#include <sys/event.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

class ClientStat;

class KqueueLoop : public IEventLoop {
public:
  KqueueLoop(std::map<int, IServer *> serverList);
  virtual ~KqueueLoop();
  void initServerSocket(); // make listening socket and register at _changeList
  void newEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                uint32_t fflags, intptr_t data, void *udata);
  //   void deleteEvent(struct kevent *event);
  void run();

private:
  KqueueLoop();
  KqueueLoop(const KqueueLoop &);
  KqueueLoop &operator=(const KqueueLoop &);

  int _kqueue;
  std::vector<struct kevent> _changeList;
  std::map<int, IServer *> _serverList;

  /* accept 성공한 fd를 index로 사용해서 client socket descriptor의 current
   * status를 저장하여 이벤트 발생시 참조 */
  // 필요한 요소는 만들면서 하나씩 추가해야 할듯,,,
  std::map<int, ClientStat *> _eventList;
  /* _eventList[currentEvent->ident]->eventExcute(struct kevent); */
};
