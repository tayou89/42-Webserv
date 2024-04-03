#ifndef CGI_EXECUTOR_HPP
#define CGI_EXECUTOR_HPP

#include "Config.hpp"
#include "Request.hpp"
#include "define.hpp"
#include "struct.hpp"
#include "utils.hpp"
#include <map>
#include <unistd.h>

class CGIExecutor {
public:
  typedef std::map<std::string, std::string> string_map;

  CGIExecutor(void);
  ~CGIExecutor(void);
  CGIExecutor(const CGIExecutor &object);
  CGIExecutor &operator=(const CGIExecutor &object);

  void _createProcess(void);

  void _createPipeGET(void);
  void _setPipeGET(void);
  struct eventStatus _executeGET(void);

  void _createPipePOST(void);
  void _setPipePOST(void);
  struct eventStatus _executePOST(void);

  int setCGIExecutor(const Request &request);
  struct eventStatus execute(void);
  int getReadFD(void) const;
  int getWriteFD(void) const;
  pid_t getPID(void) const;

private:
  void _setMetaVariables(void);
  char **_getEnvp(void) const;

  std::string _getRequestMethod(void) const;
  std::string _getDocumentRoot(void) const;
  std::string _getScriptName(void) const;
  std::string _getScriptFileName(void) const;
  std::string _getPathInfo(void) const;
  std::string _getQueryString(void) const;
  std::string _getContentType(void) const;
  std::string _getContentLength(void) const;
  std::string _getServerProtocol(void) const;

  Location _location;
  Request _request;
  string_map _metaVariables;
  int _nonBlockRead[2];
  int _nonBlockWrite[2];
  pid_t _pid;
};

#endif
