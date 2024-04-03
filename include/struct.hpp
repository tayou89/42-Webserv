#pragma once

#include "define.hpp"
#include <cstdint>

struct eventStatus {
  int status;
  int ident;
  int sub;
};

struct eventInfo {
  int socket;
  int type;
};
