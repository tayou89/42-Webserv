#pragma once

#include "Session.hpp"
#include "utils.hpp"
#include <algorithm>
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <unistd.h>
#include <vector>

class SessionControl {
public:
  SessionControl();
  ~SessionControl();

  std::string makeNewSID();
  Session *getSession(std::string sessionID);
  void deleteSession(std::string sessionID);
  void controlSession(std::map<std::string, std::string> header,
                      std::string URI);
  Session *createSession(std::string sessionID, std::string color,
                         std::string size);
  void parseURI(std::string URI);
  void makeBody(std::string color, std::string size);
  void setCookieHeader(std::string rawCookie);
  std::string getSIDcolor(std::string SID);
  std::string getSIDsize(std::string SID);
  void updateSID(std::string SID, std::string color, std::string size);
  std::string getresBody() const;
  std::string getresSessionHeaderString() const;

private:
  SessionControl(const SessionControl &copy);
  SessionControl operator=(const SessionControl &copy);

  std::vector<Session *> _sessionList;
  bool _queryStringExistance;
  bool _SIDHeaderExistance;
  std::map<std::string, std::string> _queryString;
  std::map<std::string, std::string> _reqCookieHeader;
  std::string _resBody;
  std::string _resSessionHeaderString;
};
