#include "../include/Response.hpp"

Response::Response() {}

Response::Response(char **envp, Config &conf) : _envp(envp), _config(conf) {}

Response::~Response() {}

Response::Response(const Response &copy) { *this = copy; }

Response Response::operator=(const Response &copy) {
  this->_envp = copy._envp;
  this->_request = copy._request;
  this->_responseHeader = copy._responseHeader;
  this->_response = copy._response;
  return (*this);
}

std::string Response::setResponse(Request request) {
  this->_request = request;
  if (request.getLocation().getReturn().size() != 0)
    throw _errorResponse.create302Response(this->_config,
                                           _request.getLocation());
  this->checkValidity();
  return (this->getResponse());
}

void Response::checkValidity() {
  DIR *dir = opendir(this->_request.getRequestURI().c_str());
  if (dir != NULL) {
    // check if autoindex is enabled
    if (this->_request.getLocation().getAutoIndex() == true) {
      std::string filelist;
      filelist = makeAutoindexBody(dir);
      this->setResponseBody(filelist);

      std::stringstream ss;
      ss << filelist.size();
      this->setResponseHeader("Content-Length", ss.str());
      this->setResponseHeader("Content-Type", "text/html");
      this->setResponseHeader("Last-Modified", getCurrentHttpDate());
      this->setResponse(this->_errorResponse.create200Response(
          this->_config.getServerName(), getResponseHeader(),
          getResponseBody()));
    } else {
      throw(_errorResponse.create404Response(_config));
    }
    closedir(dir);
  } else {
    if (_request.getRequestURI().find("cookie") != std::string::npos) {
      _cookie.controlCookies(_request.getRequestHeader(),
                             _request.getQueryStirng());
      this->setResponseHeader("Content-Length",
                              intToString(_cookie.getresBody().size()));
      this->setResponseHeader("Content-Type", "text/html; charset=UTF-8");
      if (_cookie.getresCookieHeaderString().size() != 0)
        this->setResponseHeader("Set-Cookie",
                                _cookie.getresCookieHeaderString());
      this->setResponseHeader("Last-Modified", getCurrentHttpDateForCookie());
      this->setResponse(this->_errorResponse.create200Response(
          this->_config.getServerName(), getResponseHeader(),
          _cookie.getresBody()));
    } else if (_request.getRequestURI().find("session") != std::string::npos) {
      _sessionControl.controlSession(_request.getRequestHeader(),
                                     _request.getQueryStirng());
      this->setResponseHeader("Content-Length",
                              intToString(_sessionControl.getresBody().size()));
      this->setResponseHeader("Content-Type", "text/html; charset=UTF-8");

      if (_sessionControl.getresSessionHeaderString().size() != 0)
        this->setResponseHeader("Set-Cookie",
                                _sessionControl.getresSessionHeaderString());
      this->setResponseHeader("Last-Modified", getCurrentHttpDateForCookie());
      this->setResponse(this->_errorResponse.create200Response(
          this->_config.getServerName(), getResponseHeader(),
          _sessionControl.getresBody()));
    } else
      this->executeMethod();
  }
}

std::string Response::makeAutoindexBody(DIR *dir) {
  struct dirent *ent;
  struct stat buf;
  std::vector<std::string> fileName;
  std::vector<std::string> fileDate;
  std::vector<std::string> fileVolume;
  while ((ent = readdir(dir)) != NULL) {
    std::stringstream ss;
    lstat(ent->d_name, &buf);
    fileName.push_back(ent->d_name);
    fileDate.push_back(getModifiedTime(buf.st_mtime));
    ss << buf.st_size;
    fileVolume.push_back(ss.str());
  }

  char readbuf[1023 + 1];
  std::string autoindexHTML;
  int fd = open("document/html/autoindex.html", O_RDONLY);
  if (fd < 0)
    throw(_errorResponse.create500Response(_config));
  memset(readbuf, 0, 1023);
  while (read(fd, readbuf, 1023) > 0) {
    std::string tmp(readbuf);
    autoindexHTML = autoindexHTML + tmp;
    memset(readbuf, 0, 1023);
  }

  std::string retBody;
  autoindexHTML = splitBefore(autoindexHTML, "{filename}") + " " +
                  _request.getRequestURI() +
                  splitAfter(autoindexHTML, "{filename}");

  retBody += splitBefore(autoindexHTML,
                         "<!-- ((this is where the body should start)) -->");
  for (int i = 0; i != static_cast<int>(fileName.size()); i++) {
    retBody += "    <div class=\"container\">\n";
    retBody += "        <div class=\"string clickable\" "
               "onclick=\"handleButtonClick('/" +
               _request.getRequestURI() + "/" + fileName[i] + "')\">" +
               fileName[i] + "</div>\n";
    retBody += "        <div class=\"string\">" + fileDate[i] + "</div>\n";
    retBody += "        <div class=\"string\">" + fileVolume[i] + "</div>\n";
    retBody += "    </div>\n\n";
  }
  retBody += splitAfter(autoindexHTML,
                        "<!-- ((this is where the body should end)) -->");
  return (retBody);
}

void Response::executeMethod() {
  if (this->_request.getRequestMethod() == "GET" ||
      this->_request.getRequestMethod() == "HEAD")
    GET_HEAD();
  else if (this->_request.getRequestMethod() == "POST")
    POST();
  else if (this->_request.getRequestMethod() == "DELETE")
    DELETE();
  else
    throw(this->_errorResponse.create405Response(this->_config));
}

char **Response::getEnvp() const { return (this->_envp); }

std::string Response::getPath(char **envp, std::string cmd) {
  std::string path;

  while (*envp) {
    if (strncmp(*envp, "PATH\0", 3) == 0) {
      std::string tmp(*envp);
      path = tmp;
      break;
    }
    envp++;
  }
  while (path.size() > 0) {
    std::string RmPath = splitBefore(path, ":") + "/" + cmd;
    if (access(RmPath.c_str(), X_OK) != -1)
      return (RmPath);
    path = splitAfter(path, ":");
  }
  throw(this->_errorResponse.create500Response(this->_config));
  return ("");
}

void Response::GET_HEAD() {
  std::vector<unsigned char> buf(BUFFER_SIZE);
  int readSize = BUFFER_SIZE;
  std::string body;

  int fd = open(this->_request.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1) {
    if (access(this->_request.getRequestURI().c_str(), F_OK) == 0)
      throw(this->_errorResponse.create403Response(_config));
    throw(this->_errorResponse.create404Response(_config));
  }

  body.clear();
  while (readSize == BUFFER_SIZE) {
    memset(&buf[0], 0, BUFFER_SIZE);
    readSize = read(fd, &buf[0], BUFFER_SIZE);
    if (readSize < 0) {
      close(fd);
      throw _errorResponse.create500Response(this->_config);
    }
    std::string tmp(buf.begin(), buf.begin() + readSize);
    body += tmp;
  }
  close(fd);
  if (this->_request.getRequestMethod() == "GET")
    this->setResponseBody(body);

  std::stringstream ss;
  ss << body.size();

  this->setResponseHeader("Content-Language", "en-US");
  this->setResponseHeader("Last-Modified", getCurrentHttpDate());
  if (_request.getRequestMethod() == "GET") {
    this->setResponseHeader("Content-Length", ss.str());
    this->setResponseHeader("Content-Type",
                            _config.getMimeType(_request.getRequestURI()) +
                                "; charset=UTF-8");
  }
  this->setResponse(this->_errorResponse.create200Response(
      this->_config.getServerName(), this->getResponseHeader(),
      this->getResponseBody()));
}

void Response::POST() { throw _errorResponse.create400Response(_config); }

void Response::DELETE() {
  int fd;

  std::vector<std::string> pathVec = splitString(_request.getRequestURI(), '/');
  std::vector<std::string>::iterator iter = pathVec.begin();

  for (; iter != pathVec.end(); iter++) {
    if (*iter == ".." || *iter == ".")
      throw(this->_errorResponse.create403Response(_config));
  }
  fd = open(this->_request.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1) {
    if (access(this->_request.getRequestURI().c_str(), F_OK) == 0)
      throw(this->_errorResponse.create403Response(_config));
    throw(this->_errorResponse.create404Response(this->_config));
  }
  close(fd);
  unlink(_request.getRequestURI().c_str());

  this->setResponseBody("");
  this->setResponseHeader("Content-Type", "text/html; charset=UTF-8");
  this->setResponseHeader("Content-Language", "en-US");
  this->setResponse(this->_errorResponse.create200Response(
      this->_config.getServerName(), this->getResponseHeader(),
      this->getResponseBody()));
}

void Response::setResponseHeader(std::string key, std::string value) {
  this->_responseHeader.insert(std::make_pair(key, value));
}

void Response::setResponseBody(std::string body) { this->_responseBody = body; }

void Response::setResponse(std::string response) { this->_response = response; }

std::map<std::string, std::string> Response::getResponseHeader() const {
  return (this->_responseHeader);
}

std::string Response::getResponseBody() const { return (this->_responseBody); }

std::string Response::getResponse() const { return (this->_response); }

void Response::initResponse() {
  _request = Request();
  _responseHeader.clear();
  _responseBody.clear();
  _response.clear();
}
