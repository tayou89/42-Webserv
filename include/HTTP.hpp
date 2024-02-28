#pragma once

#include <iostream>
#include <map>
#include <string>

class HTTP {

public:
  HTTP();
  HTTP(std::string str);
  HTTP(const HTTP &ref);
  HTTP &operator=(const HTTP &ref);

private:
  std::string _raw;
  std::map<std::string, std::string> _requestHeader;
  std::string _requestBody;
  std::string _requestMethod;
  std::string _requestURI;

  std::map<std::string, std::string> _responseHeader;
  std::string _responseBody;
  std::string _response;
};
