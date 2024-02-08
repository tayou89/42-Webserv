#include "../include/KqueueLoop.hpp"

KqueueLoop::KqueueLoop() {
  this->_kqueue = kqueue();
  if (this->_kqueue == -1)
    exit(1); // exit when error occurs
  fcntl(_kqueue, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

KqueueLoop::~KqueueLoop() {}

KqueueLoop::KqueueLoop(const KqueueLoop &ref) { static_cast<void>(ref); }

KqueueLoop &KqueueLoop::operator=(const KqueueLoop &ref) {
  if (this == &ref)
    return (*this);
  return (*this);
}

void KqueueLoop::addEvent(IEvent *event) { this->_changeList.push_back(event); }

void KqueueLoop::run() {
  while (1) {
  }
}
