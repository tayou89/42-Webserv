#include "../include/Convert.hpp"

void Convert::GET_HEAD() {
  int fd;

  char readbuf[10000];
  fd = open(this->_protocol.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1)
    throw(this->_protocol.create404Response());
  int readSize = read(fd, readbuf, 10000);
  if (readSize == -1)
    throw(this->_protocol.create500Response());
  readbuf[readSize] = '\0';
  if (this->_protocol.getRequestMethod() == "GET")
    this->_protocol.setResponseBody(readbuf);
  this->_protocol.setResponseHeader("Content-Length", std::to_string(readSize));
  // this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
  this->_protocol.setResponseHeader("Content-Type", "text/html");
  this->_protocol.setResponseHeader("Content-Language", "en-US");
  this->_protocol.setResponseHeader("Last-Modified", getCurrentHttpDate());
  this->_protocol.create200Response();
}

void Convert::POST() {
  // straight to CGI (make two pipes, then fork)
  std::cout << "POST" << std::endl;
  // return 201 created
}

std::string Convert::getPath(char **envp, std::string cmd) {
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
  // cannot find cmd, so 500 internal Convert error
  throw(this->_protocol.create500Response());
  return ("");
}

void Convert::DELETE() {
  int fd;

  fd = open(this->_protocol.getRequestURI().c_str(), O_RDONLY);
  if (fd == -1)
    throw(this->_protocol
              .create204Response()); // file does not exist, thus cannot be
                                     // deleted, 204 No Content
  close(fd);
  std::string rmPath = getPath(envp, "rm");

  // leak warning
  int pid = fork();
  if (pid < 0)
    throw(this->_protocol.create500Response());
  else if (pid == 0) {
    char **option = (char **)malloc(sizeof(char *) * 3);
    option[0] = strdup("rm\0");
    option[1] = strdup(this->_protocol.getRequestURI().c_str());
    option[2] = NULL;
    execve(rmPath.c_str(), option, this->envp);
    // free(option[0]);
    // free(option[1]);
    // free(option);
  }
  waitpid(pid, NULL, 0);

  this->_protocol.setResponseBody("");
  // this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
  this->_protocol.setResponseHeader("Content-Type", "text/html");
  this->_protocol.setResponseHeader("Content-Language", "en-US");
  this->_protocol.create200Response();
}

void Convert::PUT() {
  int fd;

  fd = open(this->_protocol.getRequestURI().c_str(), O_WRONLY | O_TRUNC);
  if (fd == -1)
    throw(this->_protocol.create404Response());
  write(fd, this->_protocol.getRequestBody().c_str(),
        this->_protocol.getRequestBody().size());
  this->_protocol.create200Response();
}

void Convert::OPTIONS() {}

void Convert::TRACE() {
  std::string body;
  std::map<std::string, std::string> header;

  body = this->_protocol.getRequestMethod() + " " +
         this->_protocol.getRequestURI() + " HTTP/1.1\r\n";
  header = this->_protocol.getRequestHeader();
  for (std::map<std::string, std::string>::iterator itr = header.begin();
       itr != header.end(); ++itr) {
    body = body + itr->first + ": " + itr->second + "\r\n";
  }
  body = body + this->_protocol.getRequestBody();
  this->_protocol.setResponseBody(body);
  this->_protocol.create200Response();
}
