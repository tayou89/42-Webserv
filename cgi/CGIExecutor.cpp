#include "../include/CGIExecutor.hpp"
#include <cerrno>
#include <cstring>
#include <iostream>

CGIExecutor::CGIExecutor(void) {}

CGIExecutor::~CGIExecutor(void) {}

CGIExecutor::CGIExecutor(const CGIExecutor &object) { *this = object; }

CGIExecutor &CGIExecutor::operator=(const CGIExecutor &object) {
  if (this == &object)
    return (*this);
  _location = object._location;
  _request = object._request;
  _metaVariables = object._metaVariables;
  _nonBlockRead[0] = object._nonBlockRead[0];
  _nonBlockRead[1] = object._nonBlockRead[1];
  _nonBlockWrite[0] = object._nonBlockWrite[0];
  _nonBlockWrite[1] = object._nonBlockWrite[1];
  _pid = object._pid;
  return (*this);
}

int CGIExecutor::setCGIExecutor(const Request &request) {
  _location = request.getLocation();
  _request = request;
  _setMetaVariables();
  if (_request.getRequestMethod() == "GET")
    return (CGI_TO_PIPE);
  else if (_request.getRequestMethod() == "POST")
    return (SERVER_TO_PIPE_WRITE);
  return (CONTINUE);
}

struct eventStatus CGIExecutor::execute(void) {
  if (_request.getRequestMethod() == "POST") {
    return (_executePOST());
  } else {
    return (_executeGET());
  }
}

void CGIExecutor::_setMetaVariables(void) {
  _metaVariables["REQUEST_METHOD"] = _getRequestMethod();
  _metaVariables["DOCUMENT_ROOT"] = _getDocumentRoot();
  _metaVariables["SCRIPT_NAME"] = _getScriptName();
  _metaVariables["SCRIPT_FILENAME"] = _getScriptFileName();
  _metaVariables["PATH_INFO"] = _getPathInfo();
  _metaVariables["QUERY_STRING"] = _getQueryString();
  _metaVariables["CONTENT_TYPE"] = _getContentType();
  _metaVariables["CONTENT_LENGTH"] = _getContentLength();
  _metaVariables["SERVER_PROTOCOL"] = std::string("HTTP/1.1");
}

std::string CGIExecutor::_getRequestMethod(void) const {
  return (_request.getRequestMethod());
}

std::string CGIExecutor::_getDocumentRoot(void) const {
  return (_location.getRootDirectory());
}

std::string CGIExecutor::_getScriptName(void) const {
  std::string locationPath = (_location.getLocationPath()).substr(1);
  std::string uri = _request.getRequestURI();
  std::size_t startIndex = uri.find(locationPath) + locationPath.size();
  std::size_t scriptEnd = ConfigUtil::findURIDelimeter(uri, startIndex);

  if (scriptEnd == std::string::npos)
    return (uri);
  else
    return (uri.substr(0, scriptEnd));
}

std::string CGIExecutor::_getScriptFileName(void) const {
  return (_metaVariables.at("DOCUMENT_ROOT") +
          _metaVariables.at("SCRIPT_NAME"));
}

std::string CGIExecutor::_getPathInfo(void) const {
  std::string uri = _request.getRequestURI();
  std::size_t scriptNameEnd = (_metaVariables.at("SCRIPT_NAME")).size();

  std::size_t pathInfoEnd = uri.find('?', scriptNameEnd);

  if (uri[scriptNameEnd] == '/')
    return (uri.substr(scriptNameEnd, pathInfoEnd));
  else
    return ("");
}

std::string CGIExecutor::_getQueryString(void) const {
  return (_request.getQueryStirng());
}

std::string CGIExecutor::_getContentType(void) const {
  std::map<std::string, std::string> headerEntry;
  std::map<std::string, std::string>::iterator iterator;

  headerEntry = _request.getRequestHeader();
  iterator = headerEntry.find("Content-Type");
  if (iterator == headerEntry.end())
    return ("");
  else
    return (iterator->second);
}

std::string CGIExecutor::_getContentLength(void) const {
  std::map<std::string, std::string> headerEntry;
  std::map<std::string, std::string>::iterator iterator;

  headerEntry = _request.getRequestHeader();
  iterator = headerEntry.find("Content-Length");
  if (iterator == headerEntry.end())
    return ("");
  else
    return (iterator->second);
}

std::string CGIExecutor::_getServerProtocol(void) const {
  return (_request.getProtocolVersion());
}

void CGIExecutor::_createProcess(void) {
  _pid = fork();
  if (_pid == -1)
    throw(std::runtime_error(std::string("fork: ") + std::strerror(errno)));
}

void CGIExecutor::_createPipeGET(void) {
  std::string path = "./tmp/" + makeTempFile();

  _nonBlockRead[WRITEFD] =
      open(path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0777);
  _nonBlockRead[READFD] = open(path.c_str(), O_RDONLY);
  unlink(path.c_str());
  if (_nonBlockRead[READFD] == -1 || _nonBlockRead[WRITEFD] == -1)
    throw(std::runtime_error(std::string("pipe: ") + std::strerror(errno)));
}

void CGIExecutor::_setPipeGET(void) {
  if (_pid == 0) {
    close(_nonBlockRead[READFD]);
    if (dup2(_nonBlockRead[WRITEFD], STDOUT_FILENO) == -1)
      throw(std::runtime_error(std::string("dup2: ") + std::strerror(errno)));
    close(_nonBlockRead[WRITEFD]);
  } else {
    fcntl(_nonBlockRead[READFD], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    close(_nonBlockRead[WRITEFD]);
  }
}

struct eventStatus CGIExecutor::_executeGET(void) {
  _createPipeGET();
  _createProcess();
  _setPipeGET();
  if (_pid == 0) {
    char **envp = _getEnvp();
    const char *path = _metaVariables["SCRIPT_FILENAME"].c_str();

    if (execve(path, NULL, envp) == -1) {
      ConfigUtil::freeStringArray(envp);
      exit(1);
    }
  }
  return (makeStatus(CGI_PROCESS, _pid));
}

void CGIExecutor::_createPipePOST(void) {
  std::string readPath = "./tmp/" + makeTempFile();
  std::string writePath = "./tmp/" + makeTempFile();

  _nonBlockRead[WRITEFD] =
      open(readPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  _nonBlockRead[READFD] = open(readPath.c_str(), O_RDONLY);
  unlink(readPath.c_str());

  _nonBlockWrite[WRITEFD] =
      open(writePath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
  _nonBlockWrite[READFD] = open(writePath.c_str(), O_RDONLY);
  unlink(writePath.c_str());

  if (_nonBlockRead[READFD] == -1 || _nonBlockRead[WRITEFD] == -1)
    throw(std::runtime_error(std::string("pipe: ") + std::strerror(errno)));
  if (_nonBlockWrite[READFD] == -1 || _nonBlockWrite[WRITEFD] == -1)
    throw(std::runtime_error(std::string("pipe: ") + std::strerror(errno)));
}

void CGIExecutor::_setPipePOST(void) {
  if (_pid == 0) {
    if (dup2(_nonBlockWrite[READFD], STDIN_FILENO) == -1)
      throw(std::runtime_error(std::string("dup2: ") + std::strerror(errno)));
    if (dup2(_nonBlockRead[WRITEFD], STDOUT_FILENO) == -1)
      throw(std::runtime_error(std::string("dup2: ") + std::strerror(errno)));
    close(_nonBlockWrite[READFD]);
    close(_nonBlockWrite[WRITEFD]);
    close(_nonBlockRead[READFD]);
    close(_nonBlockRead[WRITEFD]);
  } else {
    fcntl(_nonBlockRead[READFD], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    fcntl(_nonBlockWrite[WRITEFD], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
    close(_nonBlockRead[WRITEFD]);
    close(_nonBlockWrite[READFD]);
  }
}

struct eventStatus CGIExecutor::_executePOST(void) {
  _createPipePOST();
  _createProcess();
  _setPipePOST();
  if (_pid == 0) {
    char **envp = _getEnvp();
    const char *path = _metaVariables["SCRIPT_FILENAME"].c_str();

    if (execve(path, NULL, envp) == -1) {
      perror("Excute fail");
      ConfigUtil::freeStringArray(envp);
      exit(1);
    }
  }
  return (makeStatus(SERVER_TO_CGI, getWriteFD(), _pid));
}

char **CGIExecutor::_getEnvp(void) const {
  char **envp = new char *[_metaVariables.size() + 1];
  string_map::const_iterator iterator;
  string_map::const_iterator endPoint = _metaVariables.end();
  std::string envString;
  std::size_t i = 0;

  for (iterator = _metaVariables.begin(); iterator != endPoint; iterator++) {
    envString = iterator->first + '=' + iterator->second;
    envp[i] = new char[envString.size() + 1];
    std::copy(envString.begin(), envString.end(), envp[i]);
    envp[i][envString.size()] = '\0';
    ++i;
  }
  envp[i] = NULL;
  return (envp);
}

pid_t CGIExecutor::getPID(void) const { return (_pid); }

int CGIExecutor::getReadFD(void) const { return (_nonBlockRead[READFD]); }

int CGIExecutor::getWriteFD(void) const { return (_nonBlockWrite[WRITEFD]); }
