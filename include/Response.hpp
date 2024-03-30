#pragma once

#include "Config.hpp"
#include "Cookie.hpp"
#include "ErrorResponse.hpp"
#include "Request.hpp"
#include "SessionControl.hpp"
#include "define.hpp"

class Response {
public:
  Response();
  Response(char **envp, Config &conf);
  ~Response();

  std::string setResponse(Request _request);
  void checkValidity();
  void executeMethod();

  char **getEnvp() const;
  std::string getPath(char **envp, std::string cmd);
  std::string makeAutoindexBody(DIR *dir);

  void GET_HEAD();
  void POST();
  void DELETE();

  void setResponseHeader(std::string key, std::string value);
  void setResponseBody(std::string body);
  void setResponse(std::string response);
  std::map<std::string, std::string> getResponseHeader() const;
  std::string getResponseBody() const;
  std::string getResponse() const;

  void initResponse();

private:
  Response(const Response &copy);
  Response operator=(const Response &copy);

  char **_envp;
  Config _config;
  Request _request;
  Cookie _cookie;
  SessionControl _sessionControl;
  std::map<std::string, std::string> _responseHeader;
  std::string _responseBody;
  std::string _response;
  ErrorResponse _errorResponse;
};
