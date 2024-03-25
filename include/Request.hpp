#pragma once

#include "Config.hpp"
#include "ErrorResponse.hpp"
#include "Location.hpp"
#include "define.hpp"
#include <dirent.h>
#include <exception>
#include <fcntl.h>
#include <iostream>
#include <map>
#include <sys/stat.h>
#include <unistd.h>

class Request {
public:
  Request();
  Request(Config conf);
  ~Request();
  Request &operator=(const Request &copy);
  Request(const Request &copy);

  void setRequest(std::string packet);
  // void	checkValidity();
  void readStartLine(std::string startLine, int large_client_header_buffers);
  void readHeader(std::string header);
  void checkValidHeader(std::string key, std::string value);
  void checkContentLength(int client_max_body_size);
  void readBody(std::vector<unsigned char> body);

  int checkBodyExistence() const;

  std::string getRequestMethod() const;
  std::string getRequestURI() const;
  std::vector<unsigned char> getRequestBody() const;
  std::map<std::string, std::string> getRequestHeader() const;
  std::string getRequestHeader(std::string key) const;
  std::string getQueryStirng() const;
  Location getLocation() const;
  void setRequestHeader(std::string key, std::string value);

  //   int extensionURI(std::string location) const;
  bool extensionURI(std::string location) const;
  size_t generalURI(std::string location) const;
  std::string combinePATH(Location target, size_t rate) const;
  void convertURI();
  void eraseRequestBody(size_t start, size_t end);

private:
  Config _config;
  std::string _request;
  std::map<std::string, std::string> _requestHeader;
  std::vector<unsigned char> _requestBody;
  std::string _requestMethod;
  std::string _requestURI;
  std::string _queryString;
  Location _location;

  errorResponse _errorResponse;
};
