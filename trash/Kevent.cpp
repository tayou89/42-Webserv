#include "../include/Kevent.hpp"

Kevent::Kevent() {}

Kevent::Kevent(unsigned int ident, int filter, unsigned int flags,
               unsigned int fflags, int data, void *udata) {
  EV_SET(&(this->_event), ident, filter, flags, fflags, data, udata);
}

Kevent::Kevent(const Kevent &ref) {
  struct kevent tmp = ref.getEvent();

  EV_SET(&(this->_event), tmp.ident, tmp.filter, tmp.flags, tmp.fflags,
         tmp.data, tmp.udata);
  //   this->event.ident = tmp.ident;
  //   this->event.filter = tmp.filter;
  //   this->event.flags = tmp.flags;
  //   this->event.fflags = tmp.fflags;
  //   this->event.data = tmp.data;
  //   this->event.udata = tmp.udata;
}

Kevent::~Kevent() {}

Kevent &Kevent::operator=(const Kevent &ref) {
  if (this == &ref)
    return (*this);

  struct kevent tmp = ref.getEvent();

  EV_SET(&(this->_event), tmp.ident, tmp.filter, tmp.flags, tmp.fflags,
         tmp.data, tmp.udata);

  //   this->event.ident = tmp.ident;
  //   this->event.filter = tmp.filter;
  //   this->event.flags = tmp.flags;
  //   this->event.fflags = tmp.fflags;
  //   this->event.data = tmp.data;
  //   this->event.udata = tmp.udata;

  return (*this);
}

struct kevent Kevent::getEvent() const { return (_event); }
