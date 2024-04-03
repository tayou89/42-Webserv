#include "../include/KqueueLoop.hpp"
#include <iostream>

KqueueLoop::KqueueLoop(std::map<int, IServer *> serverList, char **envp)
    : _serverList(serverList) {
  this->_kqueue = kqueue();
  if (this->_kqueue == -1)
    exit(1);
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

void KqueueLoop::disconnect(int socket) {
  newEvent(socket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  newEvent(socket, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  delete _clientList[socket];
  _clientList[socket] = NULL;
  _clientList.erase(socket);
  close(socket);
  usleep(500);
}

void KqueueLoop::eventHandler(struct kevent *event) {
  struct eventStatus result;
  int socket;
  int type;

  struct eventInfo *info = static_cast<struct eventInfo *>(event->udata);
  if (info == NULL) {
    socket = event->ident;
    type = SOCKET;
  } else {
    socket = info->socket;
    type = info->type;
  }

  result = _clientList[socket]->eventProcess(event, type);
  if (result.status == DISCONNECT && type == SOCKET) {
    disconnect(result.ident);
  } else if (result.status == SERVER_TO_CGI) {
    newEvent(result.ident, EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0,
             _clientList[socket]->getEventInfo());
    newEvent(result.sub, EVFILT_PROC, EV_ADD | EV_ENABLE | EV_ONESHOT,
             NOTE_EXIT, 0, _clientList[socket]->getEventInfo());
    newEvent(socket, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
  } else if (result.status == CGI_PROCESS) {
    newEvent(result.ident, EVFILT_PROC, EV_ADD | EV_ENABLE | EV_ONESHOT,
             NOTE_EXIT, 0, _clientList[socket]->getEventInfo());
    newEvent(socket, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
  } else if (result.status == CGI_READ) {
    newEvent(result.ident, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0,
             _clientList[socket]->getEventInfo());
    if (result.sub > -1)
      newEvent(result.ident, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  } else if (result.status == CGI_WRITE) {
    newEvent(result.ident, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    newEvent(socket, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  } else if (result.status == SOCKET_WRITE_MODE) {
    newEvent(result.ident, EVFILT_READ, EV_DISABLE, 0, 0, NULL);
    newEvent(result.ident, EVFILT_WRITE, EV_ENABLE, 0, 0, NULL);
  } else if (result.status == SOCKET_READ_MODE) {
    _clientList[result.ident]->clearSocket();
    newEvent(result.ident, EVFILT_READ, EV_ENABLE, 0, 0, NULL);
    newEvent(result.ident, EVFILT_WRITE, EV_DISABLE, 0, 0, NULL);
  }
}

void KqueueLoop::run() {
  int eventCount;
  int eventStatus;
  struct kevent newEvents[16];
  struct kevent *currentEvent;

  while (1) {
    eventCount = kevent(_kqueue, &_changeList[0], _changeList.size(), newEvents,
                        16, NULL);
    _changeList.clear();
    if (eventCount == -1 || eventCount == 0) {
      continue;
    }

    for (int idx = 0; idx < eventCount; idx++) {
      currentEvent = &newEvents[idx];
      eventStatus = 0;
      if (currentEvent->flags & EV_ERROR) {
        continue;
      }

      if (currentEvent->filter == EVFILT_READ &&
          _serverList.find(currentEvent->ident) != _serverList.end()) {
        int newClient = accept(currentEvent->ident, NULL, NULL);
        if (newClient == -1) {
          continue;
        }
        _clientList[newClient] = new ClientSocket(
            newClient, _serverList[currentEvent->ident], _envp);
        newEvent(newClient, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
        newEvent(newClient, EVFILT_WRITE, EV_ADD | EV_DISABLE, 0, 0, NULL);
      } else {
        eventHandler(currentEvent);
      }
    }
  }
}
