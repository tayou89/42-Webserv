#include "../include/SessionControl.hpp"

SessionControl::SessionControl() {
  std::string sessionID = "defaultSession";
  _sessionList.push_back(createSession(sessionID, "000000", "15"));
}

SessionControl::SessionControl(const SessionControl &copy) {
  this->_sessionList = copy._sessionList;
  this->_queryStringExistance = copy._queryStringExistance;
  this->_queryString = copy._queryString;
  this->_reqCookieHeader = copy._reqCookieHeader;
  this->_resBody = copy._resBody;
  this->_resSessionHeaderString = copy._resSessionHeaderString;
}

SessionControl SessionControl::operator=(const SessionControl &copy) {
  this->_sessionList = copy._sessionList;
  this->_queryStringExistance = copy._queryStringExistance;
  this->_queryString = copy._queryString;
  this->_reqCookieHeader = copy._reqCookieHeader;
  this->_resBody = copy._resBody;
  this->_resSessionHeaderString = copy._resSessionHeaderString;
  return (*this);
}

SessionControl::~SessionControl() {
  for (std::vector<Session *>::iterator itr = _sessionList.begin();
       itr != _sessionList.end(); ++itr)
    delete (*itr);
}

std::string SessionControl::makeNewSID() {
  char random_name[11];
  int random_fd;
  int i;

  i = 0;

  memset(&random_name[0], 0, 11);
  random_fd = open("/dev/urandom", O_RDONLY);
  if (read(random_fd, random_name, 10) == -1)
    ;
  close(random_fd);
  while (10 > i) {
    if (random_name[i] < 0)
      random_name[i] = random_name[i] * -1;
    random_name[i] = random_name[i] % 93 + '!';
    i++;
  }
  std::string cookieName(random_name);
  return (cookieName);
}

Session *SessionControl::createSession(std::string sessionID, std::string color,
                                       std::string size) {
  // heap allocation, memory leak possible
  Session *newSession = new Session(sessionID);
  newSession->addSessionMap("color", color);
  newSession->addSessionMap("size", size);
  return (newSession);
}

Session *SessionControl::getSession(std::string sessionID) {
  std::vector<Session *>::iterator itr = _sessionList.begin();

  for (; itr != _sessionList.end(); ++itr) {
    if ((*itr)->getSessionID() == sessionID)
      return (*itr);
  }
  return (*(_sessionList.begin()));
}

void SessionControl::deleteSession(std::string sessionID) {
  std::vector<Session *>::iterator itr = _sessionList.begin();

  for (; itr != _sessionList.end(); ++itr) {
    if ((*itr)->getSessionID() == sessionID) {
      _sessionList.erase(itr);
      return;
    }
  }
  return;
}

void SessionControl::controlSession(std::map<std::string, std::string> header,
                                    std::string URI) {
  std::map<std::string, std::string>::iterator itrHeader =
      header.find("Cookie");
  parseURI(URI);
  if (itrHeader != header.end())
    setCookieHeader(itrHeader->second);
  if (_queryStringExistance == 1 &&
      _SIDHeaderExistance == 1) // 쿼리스트링 o, SID o
  {
    updateSID(_reqCookieHeader["SID"], _queryString["color"],
              _queryString["size"]);
    _resSessionHeaderString.clear();
    makeBody(_queryString["color"], _queryString["size"]);
  } else if (_queryStringExistance == 1 &&
             _SIDHeaderExistance == 0) // 쿼리스트링 o, SID x
  {
    std::string newSID = makeNewSID();
    _sessionList.push_back(
        createSession(newSID, _queryString["color"], _queryString["size"]));
    _resSessionHeaderString.clear();
    _resSessionHeaderString = "SID=" + newSID + "; ";
    makeBody(_queryString["color"], _queryString["size"]);
  } else if (_queryStringExistance == 0 &&
             _SIDHeaderExistance == 1) // 쿼리스트링 x, SID o
  {
    std::string SID = _reqCookieHeader.find("SID")->second;
    if (getSession(SID)->getSessionID() == "defaultSession")
      _sessionList.push_back(createSession(SID, "000000", "15"));
    std::string color = getSIDcolor(SID);
    std::string size = getSIDsize(SID);
    _resSessionHeaderString.clear();
    makeBody(color, size);
  } else // 쿼리스트링 x, SID x
  {
    std::string newSID = makeNewSID();
    _sessionList.push_back(createSession(newSID, "000000", "15"));
    _resSessionHeaderString.clear();
    _resSessionHeaderString = "SID=" + newSID + "; ";
    makeBody("000000", "15");
  }
}

void SessionControl::parseURI(std::string URI) {
  if (URI.find("color=") == std::string::npos &&
      URI.find("size=") == std::string::npos)
    _queryStringExistance = 0;
  else {
    _queryStringExistance = 1;
    std::string query = splitAfter(URI, "color=%23");
    std::string colorString = splitBefore(query, "&size=");
    std::string sizeString = splitAfter(query, "&size=");
    _queryString["color"] = colorString;
    _queryString["size"] = sizeString;
  }
}

void SessionControl::makeBody(std::string color, std::string size) {
  int fd = open("document/html/session.html", O_RDONLY);
  std::string cookieHTML;
  char buf[BUFFER_SIZE + 1];

  memset(buf, 0, BUFFER_SIZE + 1);
  while (read(fd, buf, BUFFER_SIZE) > 0) {
    std::string tmp(buf);
    cookieHTML += tmp;
    memset(buf, 0, BUFFER_SIZE + 1);
  }
  std::vector<std::string> rgbValue = rgbToDecimal(color);
  _resBody = splitBefore(cookieHTML, ".cookie-text") + "  .cookie-text";
  _resBody += " {\n    color: rgb(" + rgbValue[0] + ", ";
  _resBody += rgbValue[1] + ", " + rgbValue[2] + ");";
  _resBody += "\n    font-size: " + size + "px;\n  }";
  _resBody += "\n</style>\n</head>\n<body>\n\n" +
              splitAfter(cookieHTML, "</style>\n</head>\n<body>");
}

void SessionControl::setCookieHeader(std::string rawCookie) {
  std::size_t index = 0;
  std::size_t pos = 0;
  std::string tmp;

  _SIDHeaderExistance = 0;
  while (1) {
    index = rawCookie.find(";", pos);
    tmp = rawCookie.substr(pos, index - pos);
    std::string key = splitBefore(tmp, "=");
    std::string value = splitAfter(tmp, "=");
    if (key == "SID")
      _SIDHeaderExistance = 1;
    this->_reqCookieHeader[key] = value;
    if (index == std::string::npos)
      break;
    pos = index + 2;
  }
}

std::string SessionControl::getSIDcolor(std::string SID) {
  std::string color = getSession(SID)->getSessionValue("color");

  if (color.size() == 0)
    return ("000000");
  else
    return (color);
}

std::string SessionControl::getSIDsize(std::string SID) {
  std::string size = getSession(SID)->getSessionValue("size");

  if (size.size() == 0)
    return ("15");
  else
    return (size);
}

void SessionControl::updateSID(std::string SID, std::string color,
                               std::string size) {
  std::vector<Session *>::iterator itr = _sessionList.begin();

  for (; itr != _sessionList.end(); ++itr) {
    if ((*itr)->getSessionID() == SID) {
      (*itr)->addSessionMap("color", color);
      (*itr)->addSessionMap("size", size);
      return;
    }
  }
  _sessionList.push_back(createSession(SID, color, size));
}

std::string SessionControl::getresBody() const { return (_resBody); }

std::string SessionControl::getresSessionHeaderString() const {
  return (_resSessionHeaderString + "\r\n");
}
