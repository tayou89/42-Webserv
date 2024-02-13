#include "../include/KqueueLoop.hpp"

KqueueLoop::KqueueLoop(std::map<int, IServer *> serverList)
    : _serverList(serverList) {
  this->_kqueue = kqueue();
  if (this->_kqueue == -1)
    exit(1); // exit when error occurs
  fcntl(_kqueue, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
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
  // register _serverList at kqueue
  std::map<int, IServer *>::iterator iter = _serverList.begin();

  for (; iter != _serverList.end(); iter++) {
    // iter->second->getListenSocket 으로 반환된 descriptor를
    // Event 객체로 변환하여 _changeList에 등록함
  }
}

void KqueueLoop::addEvent(Kevent event) {
  this->_changeList.push_back(event.getEvent());
}

void KqueueLoop::run() {
  int events;
  struct kevent newEvents[8];
  struct kevent *currentEvent;

  while (1) {
    events = kevent(_kqueue, &_changeList[0], _changeList.size(), newEvents, 8,
                    NULL);
    if (events == -1)
      exit(1);           // kevent error exit
    _changeList.clear(); // clear list of new register events

    for (int idx = 0; idx < events; idx++) {
      currentEvent = &newEvents[idx];
      if (currentEvent->flags & EV_ERROR)
        exit(1); // server or client socket error exit
      else if (currentEvent->filter == EVFILT_READ) {
        // read available event occurs
        if (_serverList.find(currentEvent->ident) != _serverList.end()) {
          // listening socket read event occurs
        } else {
          // client socket read event occurs
        }
      } else if (currentEvent->filter == EVFILT_WRITE) {
        // send data to client available
      }
    }
  }
}
