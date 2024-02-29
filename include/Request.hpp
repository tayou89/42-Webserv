#pragma once

#include "ErrorResponse.hpp"
#include "define.hpp"
#include <dirent.h>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <unistd.h>

class Request {
public:
  Request();
  ~Request();
  Request &operator=(const Request &copy);
  Request(const Request &copy);

  void setRequest(std::string packet);
  // void	checkValidity();
  void readStartLine(std::string startLine, int large_client_header_buffers);
  void readHeader(std::string header);
  void checkValidHeader(std::string key, std::string value);
  void checkContentLength(int client_max_body_size);
  void readBody(std::string body);

  int checkBodyExistence() const;

  std::string getRequestMethod() const;
  std::string getRequestURI() const;
  std::string getRequestBody() const;
  std::map<std::string, std::string> getRequestHeader() const;
  std::string getRequestHeader(std::string key) const;
  void setRequestHeader(std::string key, std::string value);

private:
  std::string _request;
  std::map<std::string, std::string> _requestHeader;
  std::string _requestBody;
  std::string _requestMethod;
  std::string _requestURI;

  errorResponse _errorResponse;
};
