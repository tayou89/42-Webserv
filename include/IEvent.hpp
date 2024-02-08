#pragma once

class IEvent {
public:
  virtual ~IEvent() {}
  virtual IEvent *setEvent() = 0;
};
