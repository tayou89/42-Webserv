#include "../include/CGIExecutor.hpp"
#include <cerrno>
#include <cstring>

CGIExecutor::CGIExecutor(void) {}

CGIExecutor::~CGIExecutor(void) {}

CGIExecutor::CGIExecutor(const CGIExecutor &object) { *this = object; }

CGIExecutor &CGIExecutor::operator=(const CGIExecutor &object) {
  if (this == &object)
    return (*this);
  _location = object._location;
  _request = object._request;
  _metaVariables = object._metaVariables;
  _pipeFD[0] = object._pipeFD[0];
  _pipeFD[1] = object._pipeFD[1];
  _pid = object._pid;
  return (*this);
}

CGIExecutor::CGIExecutor(const Request &request)
    : _location(request.getLocation()), _request(request) {
  _setMetaVariables();
}

// int CGIExecutor::execute(void) {
//   try {
//     _createPipeFD();
//     _createProcess()
//     _setPipeFD();
//     _executeCGI();
//   } catch (const std::exception &e) {
//     std::cerr << "Error: " << e.what() << '\n';
//     if (_pid == 0)
//       exit(1);
//     else
//       return (1);
//   }
//   if (_pid == 0)
//     exit(0);
//   else
//     return (0);
// }

void CGIExecutor::_setMetaVariables(void) {
  _metaVariables["REQUEST_METHOD"] = _getRequestMethod();
  _metaVariables["DOCUMENT_ROOT"] = _getDocumentRoot();
  _metaVariables["SCRIPT_NAME"] = _getScriptName();
  _metaVariables["SCRIPT_FILENAME"] = _getScriptFileName();
  _metaVariables["PATH_INFO"] = _getPathInfo();
  _metaVariables["QUERY_STRING"] = _getQueryString();
  _metaVariables["CONTENT_TYPE"] = _getContentType();
  _metaVariables["CONTENT_LENGTH"] = _getContentLength();
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
  size_t startIndex = uri.find(locationPath) + locationPath.size();
  size_t scriptEnd = ConfigUtil::findURIDelimeter(uri, startIndex);

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
  size_t scriptNameEnd = (_metaVariables.at("SCRIPT_NAME")).size();

  if (uri[scriptNameEnd] == '/')
    return (uri.substr(scriptNameEnd));
  else
    return ("");
}

std::string CGIExecutor::_getQueryString(void) const {
  std::string uri = _request.getRequestURI();
  size_t scriptNameEnd = (_metaVariables.at("SCRIPT_NAME")).size();

  if (uri[scriptNameEnd] == '?')
    return (uri.substr(scriptNameEnd));
  else
    return ("");
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

void CGIExecutor::createPipeFD(void) {
  if (pipe(_pipeFD) == -1)
    throw(std::runtime_error(std::string("pipe: ") + std::strerror(errno)));
	fcntl(_pipeFD[READFD], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
	fcntl(_pipeFD[WRITEFD], F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

void CGIExecutor::createProcess(void) {
  _pid = fork();
  if (_pid == -1)
    throw(std::runtime_error(std::string("fork: ") + std::strerror(errno)));
}

void CGIExecutor::setPipeFD(void) {
  if (_pid == 0) {
    close(_pipeFD[0]);
    if (dup2(_pipeFD[1], STDOUT_FILENO) == -1)
      throw(std::runtime_error(std::string("dup2: ") + std::strerror(errno)));
    close(_pipeFD[1]);
  } else
    close(_pipeFD[1]);
}

void CGIExecutor::executeCGI(void) {
//   if (_pid == 0) {
//     ConfigFile CGIFile = _location.getIndexFile();
//     char **envp = _getEnvp();
//     char *argv[2] = {const_cast<char *>(CGIFile.getPath().c_str()), NULL};

//     if (execve(argv[0], argv, envp) == -1) {
//       ConfigUtil::freeStringArray(envp);
//       throw(std::runtime_error(std::string("execve: ") + std::strerror(errno)));
//     }
//   }
}

char **CGIExecutor::_getEnvp(void) const {
  char **envp = new char *[_metaVariables.size() + 1];
  string_map::const_iterator iterator;
  string_map::const_iterator endPoint = _metaVariables.end();
  std::string envString;
  size_t i = 0;

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

int CGIExecutor::getReadFD(void) const { return (_pipeFD[READFD]); }
int CGIExecutor::getWriteFD(void) const { return (_pipeFD[WRITEFD]); }
