#include "../include/Response.hpp"

Response::Response() {}

Response::Response(char **envp, Config conf) : _envp(envp), _config(conf) {}

Response::~Response() {}

Response::Response(const Response &copy) { *this = copy; }

Response Response::operator=(const Response &copy) {
  this->_envp = copy._envp;
  this->_request = copy._request;
  this->_responseHeader = copy._responseHeader;
  this->_response = copy._response;
  // this->_errorResponse = copy._errorResponse;
  return (*this);
}

std::string Response::setResponse(Request _request) {
  this->_request = _request;
  this->checkValidity();
  // this->sendResponse();
  return (this->getResponse());
}

void Response::checkValidity() {
  DIR *dir = opendir(this->_request.getRequestURI().c_str());
  if (dir != NULL) {
    // 1. check index directive in conf file
    // if (this->_config.getIndex() == true)
    // {
    // 	//find index files in order, then put it in the response packet
    // }

    // 2. check if autoindex is enabled
    if (this->_config.getLocation(this->_request.getRequestURI()).getAutoIndex() == true)
    {
    	//read all files in the directory and put it in the response packet
    	struct dirent *ent;
    	std::string filelist = "";
    	while ((ent = readdir(dir)) != NULL)
    		filelist = filelist + ent->d_name + "\n";
    	this->setResponseBody(filelist);
    	this->setResponseHeader("Content-Length",
    std::to_string(filelist.size()));
    	// this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
    	this->setResponseHeader("Content-Type", "directory Listing");
    //need to change
    this->setResponseHeader("Last-Modified", getCurrentHttpDate());
    this->_errorResponse.create200Response(getResponseHeader(), getResponseBody());
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
    this->executeMethod();
    // check for cookie
  }
}

void Response::executeMethod() {
  if (this->_request.getRequestMethod() == "GET" ||
      this->_request.getRequestMethod() == "HEAD")
    GET_HEAD();
  else if (this->_request.getRequestMethod() == "POST")
    POST();
  else if (this->_request.getRequestMethod() == "DELETE")
    DELETE();
  else if (this->_request.getRequestMethod() == "PUT")
    PUT();
  else if (this->_request.getRequestMethod() == "OPTIONS")
    OPTIONS();
  else if (this->_request.getRequestMethod() == "TRACE")
    TRACE();
  else
    throw(this->_errorResponse.create405Response());
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
    std::string RmPath = splitBeforeColon(path) + "/" + cmd;
    if (access(RmPath.c_str(), X_OK) != -1)
      return (RmPath);
    path = splitAfterColon(path);
  }
  // cannot find cmd, so 500 internal server error
  throw(this->_errorResponse.create500Response());
  return ("");
}

void Response::GET_HEAD() {
  char readbuf[10000];
  _responseFile = open(this->_request.getRequestURI().c_str(), O_RDONLY);
  if (_responseFile == -1)
    throw(this->_errorResponse.create404Response());
  int readSize = read(_responseFile, readbuf, 10000);
  if (readSize == -1)
    throw(this->_errorResponse.create500Response());
  readbuf[readSize] = '\0';
  if (this->_request.getRequestMethod() == "GET")
    this->setResponseBody(readbuf);
  this->setResponseHeader("Content-Length", std::to_string(readSize));
  // this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
  this->setResponseHeader("Content-Type", "text/html");
  this->setResponseHeader("Content-Language", "en-US");
  this->setResponseHeader("Last-Modified", getCurrentHttpDate());
  this->setResponse(this->_errorResponse.create200Response(
      this->getResponseHeader(), this->getResponseBody()));
}

void Response::POST() {
  // straight to CGI (make two pipes, then fork)
  std::cout << "POST" << std::endl;
  // return 201 created
}

void Response::DELETE() {
  int fd;

  fd = open(this->_request.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1)
    throw(this->_errorResponse
              .create204Response()); // file does not exist, thus cannot be
                                     // deleted, 204 No Content
  close(fd);
  std::string rmPath = getPath(this->getEnvp(), "rm");

  // leak warning
  int pid = fork();
  if (pid < 0)
    throw(this->_errorResponse.create500Response());
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
      this->getResponseHeader(), this->getResponseBody()));
}

void Response::PUT() {
  int fd;

  fd = open(this->_request.getRequestURI().c_str(), O_WRONLY | O_TRUNC);
  if (fd == -1)
    throw(this->_errorResponse.create404Response());
  write(fd, this->_request.getRequestBody().c_str(),
        this->_request.getRequestBody().size());
  this->setResponse(this->_errorResponse.create200Response(
      this->getResponseHeader(), this->getResponseBody()));
}

void Response::OPTIONS() {
  if (this->_request.getRequestHeader("Origins") == "")
    throw(this->_errorResponse.create400Response());
  int fd = open(this->_request.getRequestHeader("Origins").c_str(), O_RDONLY);
  if (fd == -1)
    throw(this->_errorResponse.create400Response());
  this->setResponseHeader("Access-Control-Allow-Origin",
                          this->_request.getRequestHeader("Origins"));
  this->setResponse(this->_errorResponse.create200Response(
      this->getResponseHeader(), this->getResponseBody()));
}

void Response::TRACE() {
  std::string body;
  std::map<std::string, std::string> header;

  body = this->_request.getRequestMethod() + " " +
         this->_request.getRequestURI() + " HTTP/1.1\r\n";
  header = this->_request.getRequestHeader();
  for (std::map<std::string, std::string>::iterator itr = header.begin();
       itr != header.end(); ++itr) {
    body = body + itr->first + ": " + itr->second + "\r\n";
  }
  body = body + this->_request.getRequestBody();
  this->setResponseBody(body);
  this->setResponse(this->_errorResponse.create200Response(
      this->getResponseHeader(), this->getResponseBody()));
}

void Response::setResponseHeader(std::string key, std::string value) {
  this->_responseHeader.insert(std::make_pair(key, value));
}

void Response::setResponseBody(std::string body) { this->_responseBody = body; }

void Response::setResponse(std::string response) { this->_response = response; }

int Response::getResponseFile() const { return (_responseFile); }

std::map<std::string, std::string> Response::getResponseHeader() const {
  return (this->_responseHeader);
}

std::string Response::getResponseBody() const { return (this->_responseBody); }

std::string Response::getResponse() const { return (this->_response); }
