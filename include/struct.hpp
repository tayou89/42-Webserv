#pragma once

#include "define.hpp"
#include <cstdint>

struct eventStatus {
  int status; // WRITE_MODE, READ_MODE, NEW_EVENT, PROCESSOR, READ_ERROR ...
  int ident;
  int sub;
};

struct eventInfo {
  int socket;
  int type;
};
