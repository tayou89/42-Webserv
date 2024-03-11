#pragma once

#include "../include/utils.hpp"
#include "Config.hpp"
#include "Location.hpp"
#include "fcntl.h"
#include "unistd.h"
#include <map>

class errorResponse {
public:
  errorResponse();
  ~errorResponse();

  std::string
  create200Response(std::string serverName,
                    std::map<std::string, std::string> responseHeader,
                    std::string responseBody);
  std::string create204Response(Config conf);
  std::string create400Response(Config conf);
  std::string create403Response(Config conf);
  std::string create404Response(Config conf);
  std::string create405Response(Config conf);
  std::string create413Response(Config conf);
  std::string create414Response(Config conf);
  std::string create500Response(Config conf);
  std::string create505Response(Config conf);

private:
  errorResponse(const errorResponse &copy);
  errorResponse operator=(const errorResponse &copy);
};
