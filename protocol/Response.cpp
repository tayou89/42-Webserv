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
    // 1. check index directive in conf file
    // if (this->_request.getLocation().getIndexFile().isAccessible() == true) {
    //   // find index files in order, then put it in the response packet
    //   //   if (this->_config.getLocation(this->_request.getRequestURI())
    //   //           .getIndexFile()
    //   //           .isReadable() == true) {
    //   std::string path =
    //       this->_config.getLocation(this->_request.getRequestURI())
    //           .getIndexFile()
    //           .getPath();
    //   int fd = open(path.c_str(), O_RDONLY);
    //   if (fd == -1)
    //     throw(this->_errorResponse.create500Response(this->_config));
    //   char buf[1000]; // need to change buffer size
    //   memset(buf, 0, 1000);
    //   std::string body;
    //   if (read(fd, buf, 1000) > 0) {
    //     std::string tmp_body(buf);
    //     body = body + tmp_body;
    //     memset(buf, 0, 1000);
    //   }
    //   close(fd);
    //   this->setResponseBody(body);
    //   this->setResponseHeader("Content-Length", std::to_string(body.size()));
    //   this->setResponseHeader("Last-Modified", getCurrentHttpDate());
    //   this->setResponse(this->_errorResponse.create200Response(
    //       this->_config.getServerName(), getResponseHeader(),
    //       getResponseBody()));
    //   //   }
    // }

    // 2. check if autoindex is enabled
    if (this->_request.getLocation().getAutoIndex() == true) {
      // read all files in the directory and put it in the response packet
      // if (1) {

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
      // auto index not available
      throw(_errorResponse.create403Response(_config));
    }
    closedir(dir);
  } else { // if URI is a file
    // 1. check if the method is allowed or executable
    // -> going to check it in executeMethod()
    // 2. check if authorization header exists
    if (this->_request.getRequestHeader("Authorization") != "") {
      // check if the user is authorized
      // if not, throw 401 response
    }
    if (_request.getRequestURI().find("cookie") != std::string::npos) {
      // std::cout << "inside cookie\n";
      _cookie.controlCookies(_request.getRequestHeader(),
                             _request.getQueryStirng());
      this->setResponseHeader("Content-Length",
                              intToString(_cookie.getresBody().size()));
      this->setResponseHeader("Content-Type", "text/html");
      if (_cookie.getresCookieHeaderString().size() != 0)
        this->setResponseHeader("Set-Cookie",
                                _cookie.getresCookieHeaderString());
      this->setResponseHeader("Last-Modified", getCurrentHttpDateForCookie());
      this->setResponse(this->_errorResponse.create200Response(
          this->_config.getServerName(), getResponseHeader(),
          _cookie.getresBody()));
      // std::cout << "\nthis is response:\n" << getResponse() << std::endl;
    } else if (_request.getRequestURI().find("session") != std::string::npos) {
      _sessionControl.controlSession(_request.getRequestHeader(),
                                     _request.getQueryStirng());
      this->setResponseHeader("Content-Length",
                              intToString(_sessionControl.getresBody().size()));
      this->setResponseHeader("Content-Type", "text/html");

      if (_sessionControl.getresSessionHeaderString().size() != 0)
        this->setResponseHeader("Set-Cookie",
                                _sessionControl.getresSessionHeaderString());
      this->setResponseHeader("Last-Modified", getCurrentHttpDateForCookie());
      this->setResponse(this->_errorResponse.create200Response(
          this->_config.getServerName(), getResponseHeader(),
          _sessionControl.getresBody()));
    } else
      this->executeMethod();
    // check for cookie
  }
}

std::string Response::makeAutoindexBody(DIR *dir) {
  struct dirent *ent;
  struct stat buf;
  std::vector<std::string> fileName;
  std::vector<std::string> fileDate;
  std::vector<std::string> fileVolume;
  // get directory file information
  while ((ent = readdir(dir)) != NULL) {
    std::stringstream ss;
    lstat(ent->d_name, &buf);
    fileName.push_back(ent->d_name);
    fileDate.push_back(getModifiedTime(buf.st_mtime));
    ss << buf.st_size;
    fileVolume.push_back(ss.str());
  }

  // read autoindex.html
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

  // make autoindex.html with directory file information
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
  // cannot find cmd, so 500 internal server error
  throw(this->_errorResponse.create500Response(this->_config));
  return ("");
}

void Response::GET_HEAD() {
  char readbuf[BUFFER_SIZE + 1];
  int readSize = BUFFER_SIZE;
  std::string body;

  int fd = open(this->_request.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1) {
    throw(this->_errorResponse.create404Response(_config));
  }

  body.clear();
  while (readSize == BUFFER_SIZE) {
    memset(readbuf, 0, BUFFER_SIZE + 1);
    readSize = read(fd, readbuf, BUFFER_SIZE);
    if (readSize < 0) {
      close(fd);
      throw _errorResponse.create500Response(this->_config);
    }
    std::string tmp(readbuf);
    body += tmp;
  }
  close(fd);
  if (this->_request.getRequestMethod() == "GET")
    this->setResponseBody(body);

  std::stringstream ss;
  ss << body.size();
  this->setResponseHeader("Content-Length", ss.str());
  this->setResponseHeader("Content-Type",
                          _config.getMimeType(_request.getRequestURI()));
  this->setResponseHeader("Content-Language", "en-US");
  this->setResponseHeader("Last-Modified", getCurrentHttpDate());
  this->setResponse(this->_errorResponse.create200Response(
      this->_config.getServerName(), this->getResponseHeader(),
      this->getResponseBody()));
}

void Response::POST() {
  // straight to CGI (make two pipes, then fork)
  std::cout << "POST" << std::endl;
  // return 201 created
}

void Response::DELETE() {
  int fd;

  std::vector<std::string> pathVec = splitString(_request.getRequestURI(), '/');
  std::vector<std::string>::iterator iter = pathVec.begin();

  for (; iter != pathVec.end(); iter++) {
    if (*iter == ".." || *iter == ".")
      throw(this->_errorResponse.create403Response(_config));
  }
  fd = open(this->_request.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1)
    throw(this->_errorResponse.create204Response(
        this->_config)); // file does not exist, thus cannot be
                         // deleted, 204 No Content
  close(fd);
  std::string rmPath = getPath(this->getEnvp(), "rm");

  // leak warning
  int pid = fork();
  if (pid < 0)
    throw(this->_errorResponse.create500Response(this->_config));
  else if (pid == 0) {
    char **option = (char **)malloc(sizeof(char *) * 3);
    option[0] = strdup("rm\0");
    option[1] = strdup(this->_request.getRequestURI().c_str());
    option[2] = NULL;
    execve(rmPath.c_str(), option, this->getEnvp());
    // free(option[0]);
    // free(option[1]);
    // free(option);
  }
  waitpid(pid, NULL, 0);

  this->setResponseBody("");
  // this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
  this->setResponseHeader("Content-Type", "text/html");
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
