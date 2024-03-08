#pragma once

#include <iostream>
#include <unistd.h>

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

// # ifndef BUFFER_SIZE
// #  define BUFFER_SIZE 1
// # endif

std::string getCurrentHttpDate();
std::string splitBefore(std::string sentence, std::string splitString);
std::string splitAfter(std::string sentence, std::string splitString);
