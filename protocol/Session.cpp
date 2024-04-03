#include "../include/Session.hpp"

Session::Session(std::string sessionID) { _sessionID = sessionID; }

Session::Session(const Session &copy) { this->_sessionID = copy._sessionID; }

Session Session::operator=(const Session &copy) {
  this->_sessionID = copy._sessionID;
  return (*this);
}

Session::~Session() {}

std::string Session::getSessionID() const { return (_sessionID); }

void Session::addSessionMap(std::string key, std::string value) {
  _sessionMap[key] = value;
}

std::map<std::string, std::string> Session::getSessionMap() const {
  return (_sessionMap);
}

std::string Session::getSessionValue(std::string key) const {
  if (_sessionMap.find(key) == _sessionMap.end()) {
    return ("");
  } else
    return (_sessionMap.find(key)->second);
}
