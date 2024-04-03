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
  Request(Config &conf);
  ~Request();
  Request &operator=(const Request &copy);
  Request(const Request &copy);

  void setRequest(std::string packet);
  void checkValidHeader(std::string key, std::string value);
  void readStartLine(std::string startLine);
  void readHeader(std::string header);
  void readBody(std::vector<unsigned char> body);

  int checkBodyExistence() const;

  std::string getRequestMethod() const;
  std::string getRequestURI() const;
  std::vector<unsigned char> getRequestBody() const;
  std::map<std::string, std::string> getRequestHeader() const;
  std::string getRequestHeader(std::string key) const;
  std::string getQueryStirng() const;
  std::string getProtocolVersion() const;
  Location getLocation() const;
  void setRequestHeader(std::string key, std::string value);

  bool extensionURI(std::string location) const;
  std::size_t generalURI(std::string location) const;
  std::string combinePATH(Location target, std::size_t rate) const;
  void convertURI();
  void eraseRequestBody(int start, int end);
  void initRequest();
  void checkRequestValidity();
  void checkRequestMethod();

private:
  Config _config;
  std::string _request;
  std::map<std::string, std::string> _requestHeader;
  std::vector<unsigned char> _requestBody;
  std::string _requestMethod;
  std::string _requestURI;
  std::string _requestHTTPVersion;
  std::string _queryString;
  Location _location;

  ErrorResponse _errorResponse;
};
