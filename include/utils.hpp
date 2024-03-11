#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

// # ifndef BUFFER_SIZE
// #  define BUFFER_SIZE 1
// # endif

std::string getCurrentHttpDate();
std::string splitBeforeColon(std::string sentence);
std::string splitAfterColon(std::string sentence);
std::vector<std::string> splitString(const std::string &input, char delimiter);
