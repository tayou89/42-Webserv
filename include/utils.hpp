#pragma once

#include "struct.hpp"
#include <algorithm>
#include <fcntl.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unistd.h>
#include <vector>

#ifndef OPEN_MAX
#define OPEN_MAX 256
#endif

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
void replace(std::string &str, const std::string &target,
             const std::string &replacement);
std::vector<unsigned char>::iterator
findHeader(std::vector<unsigned char> &vec);
std::vector<unsigned char>::iterator
findNewLine(std::vector<unsigned char> &vec);
std::vector<unsigned char>::iterator
findNewLineNewLine(std::vector<unsigned char> &vec);
std::string makeTempFile();
