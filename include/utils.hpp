#pragma once

#include "struct.hpp"
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
std::string getCurrentHttpDateForCookie();
std::string splitBefore(std::string sentence, std::string splitWord);
std::string splitAfter(std::string sentence, std::string splitWord);
std::vector<std::string> splitString(const std::string &input, char delimiter);
struct eventStatus makeStatus(int status, int ident);
struct eventStatus makeStatus(int status, int ident, int sub);
std::string getModifiedTime(time_t mTime);
std::vector<std::string> rgbToDecimal(const std::string &rgbHex);
std::string decimalToRgb(int red, int green, int blue);
std::string intToString(int value);
