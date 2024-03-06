#include "../include/KqueueLoop.hpp"
#include <iostream>

KqueueLoop::KqueueLoop(std::map<int, IServer *> serverList, char **envp)
    : _serverList(serverList) {
  this->_kqueue = kqueue();
  if (this->_kqueue == -1)
    exit(1); // exit when error occurs
  fcntl(_kqueue, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
  _envp = envp;
}

KqueueLoop::KqueueLoop() {}

KqueueLoop::~KqueueLoop() {}

KqueueLoop::KqueueLoop(const KqueueLoop &ref) { static_cast<void>(ref); }

KqueueLoop &KqueueLoop::operator=(const KqueueLoop &ref) {
  if (this == &ref)
    return (*this);
  return (*this);
}

void KqueueLoop::initServerSocket() {
  std::map<int, IServer *>::iterator iter = _serverList.begin();

  for (; iter != _serverList.end(); iter++)
    newEvent(iter->first, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
}

void KqueueLoop::newEvent(uintptr_t ident, int16_t filter, uint16_t flags,
                          uint32_t fflags, intptr_t data, void *udata) {
  struct kevent temp;

  EV_SET(&temp, ident, filter, flags, fflags, data, udata);
  _changeList.push_back(temp);
}

// void KqueueLoop::deleteEvent(struct kevent *event) {
//   struct kevent temp;

//   EV_SET(&temp, event->ident, event->filter, event->flags, event->fflags,
//          event->data, event->udata);
//   _changeList.push_back(temp);
// }

void KqueueLoop::disconnect(int socket) {
  newEvent(socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  newEvent(socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  delete _clientList[socket]; // allocation delete
  _clientList[socket] = NULL;
  _clientList.erase(socket);
  close(socket);
  std::cout << "disconnect: " << socket << "\n";
  if (_clientList.find(socket) == _clientList.end())
    std::cout << "successfully disconnected\n";
}

void KqueueLoop::run() {
  int eventCount;
  int eventStatus;
  struct kevent newEvents[1024];
  struct kevent *currentEvent;

  while (1) {
    eventCount = kevent(_kqueue, &_changeList[0], _changeList.size(), newEvents,
                        1024, NULL);
    if (eventCount == -1)
      exit(1);           // kevent error exit
    _changeList.clear(); // clear list of new register events

    for (int idx = 0; idx < eventCount; idx++) {
      currentEvent = &newEvents[idx];
      eventStatus = 0;
      if (currentEvent->flags & EV_ERROR)
        continue; // skip the error event

      if (_serverList.find(currentEvent->ident) != _serverList.end()) {
        int newClient = accept(currentEvent->ident, NULL, NULL);
        if (newClient == 0)
          continue;
        fcntl(newClient, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
        _clientList[newClient] = new ClientSocket(
            newClient, _serverList[currentEvent->ident], _envp);
        if (newClient == -1)
          exit(1); // accept error
        newEvent(newClient, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        newEvent(newClient, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
        std::cout << "hello " << newClient << std::endl;
      } else {
        if (currentEvent->filter == EVFILT_READ) {
          eventStatus = _clientList[currentEvent->ident]->readSocket();
          if (eventStatus == WRITE_MODE) { // change event status
            std::cout << "write mode\n";
            newEvent(currentEvent->ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
            newEvent(currentEvent->ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
          }
        } else if (currentEvent->filter == EVFILT_WRITE) {
          eventStatus = _clientList[currentEvent->ident]->writeSocket();
          if (eventStatus == READ_MODE) { // write finish, change status
            std::cout << "read mode\n";
            _clientList[currentEvent->ident]->clearSocket();
            newEvent(currentEvent->ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
            newEvent(currentEvent->ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
          }
        }

        /* connection ended */
        if (eventStatus == DISCONNECT) {
          disconnect(currentEvent->ident);
        } else if (eventStatus == WRITE_ERROR) {
          /* Write Event Error Process */
          std::cout << "Write Error occurs\n";
        }
        /* 서버 내부 동작 구현 */
        // timeout exception 먼저 처리 (Event객체 내부구현)
        // client socket event occurs (READ or WRITE)
        // One Action per One Event
        // return type register new event?
        // Udata *ptr = static_cast<Udata *>(currentEvent->udata) 이용?
        // Server class 에서 이벤트 처리하려면 udata 이용하는편이 좋을수도?
        // 이벤트 객체에 *udata를 server class instance로 활용하면 편할수도?
        // 이벤트 실행을 구현하면서 둘중에 용의한 방법을 취사선택 해야될듯
        // read size가 0이면 클라이언트 소켓의 연결이 종료된 상태
        // fd(ident)를 index값으로 가지는 map에 fd별로 Kevent객체를 저장
        // Kevent instance에 서버가 저장되어 있으므로 행동을 정의하기 용이하고
        // fd의 현재 상태를 표시하기 좋음 (timeout, readable, eof 등등)
      }
    }
  }
}
