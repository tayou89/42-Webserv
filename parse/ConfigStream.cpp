#include "../include/ConfigStream.hpp"
#include <iostream>
#include <iterator>

ConfigStream::ConfigStream(void) {}

ConfigStream::~ConfigStream(void) {}

ConfigStream::ConfigStream(const ConfigStream &object) { *this = object; }

ConfigStream &ConfigStream::operator=(const ConfigStream &object) {
  if (this == &object)
    return (*this);
  init();
  *this << object.rdbuf();
  return (*this);
}

ConfigStream::ConfigStream(std::string configText) {
  ConfigUtil::removeComments(configText);
  *this << configText;
}

void ConfigStream::init(void) {
  str("");
  clear();
}

void ConfigStream::pushFront(const std::string &string) {
  std::string previousBuffer((std::istreambuf_iterator<char>(*this)),
                             std::istreambuf_iterator<char>());
  std::string newBuffer = string + previousBuffer;

  init();
  *this << newBuffer;
}
