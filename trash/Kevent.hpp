#pragma once

#include "IEvent.hpp"
#include <sys/event.h>

class Kevent : public IEvent {

public:
  Kevent();
  Kevent(unsigned int ident, int filter, unsigned int flags,
         unsigned int fflags, int data, void *udata);
  Kevent(const Kevent &ref);
  ~Kevent();
  Kevent &operator=(const Kevent &ref);

  struct kevent getEvent() const;

private:
  struct kevent _event;
};
