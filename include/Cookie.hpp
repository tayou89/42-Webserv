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

class Cookie {
public:
  Cookie();
  ~Cookie();
  void controlCookies(std::map<std::string, std::string> header,
                      std::string URI);
  time_t getCookieTime(int day, int hour, int min);
  std::string convertIntoRealTime(time_t rawtime);

  void setCookieHeader(std::string rawCookie);
  void parseURI(std::string URI);
  void makeBody(std::string color, std::string size);

  std::string getresBody() const;
  std::string getresCookieHeaderString() const;

private:
  Cookie(const Cookie &copy);
  Cookie operator=(const Cookie &copy);

  std::map<std::string, std::string> _reqCookieHeader;
  std::map<std::string, std::string> _resCookieHeader;
  std::map<std::string, std::string> _queryString;
  bool _queryStringExistance;
  bool _cookieExistance;
  std::string _resCookieHeaderString;
  std::string _resBody;
};
