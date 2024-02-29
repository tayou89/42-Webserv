#include "../include/ErrorResponse.hpp"

////////////////////////////////////////////
// you MUST change the servername!!!!!!!!!!!!
////////////////////////////////////////////

errorResponse::errorResponse() {}

errorResponse::~errorResponse() {}

errorResponse::errorResponse(const errorResponse &copy) { (void)copy; }

errorResponse errorResponse::operator=(const errorResponse &copy) {
  (void)copy;
  return (*this);
}

std::string errorResponse::create200Response(
    std::map<std::string, std::string> responseHeader,
    std::string responseBody) {
  std::string response =
      "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() + "Server: " + "webserv\r\n";
  for (std::map<std::string, std::string>::iterator itr =
           responseHeader.begin();
       itr != responseHeader.end(); ++itr)
    response = response + itr->first + ": " + itr->second + "\r\n";
  response = response + "\r\n" + responseBody;
  // this->_response = response;
  return (response);
}

std::string errorResponse::create204Response() {
  std::string response = "HTTP/1.1 204 No Content\r\n" + getCurrentHttpDate() +
                         "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create400Response() {
  std::string response = "HTTP/1.1 400 Bad Request\r\n" + getCurrentHttpDate() +
                         "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create404Response() {
  std::string response = "HTTP/1.1 404 Not Found\r\n" + getCurrentHttpDate() +
                         "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create405Response() {
  std::string response = "HTTP/1.1 405 Method Not Allowed\r\n" +
                         getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create413Response() {
  std::string response = "HTTP/1.1 413 Request Entity Too Large\r\n" +
                         getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create414Response() {
  std::string response = "HTTP/1.1 414 URI Too Long\r\n" +
                         getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create500Response() {
  std::string response = "HTTP/1.1 500 Internal Server Error\r\n" +
                         getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}

std::string errorResponse::create505Response() {
  std::string response = "HTTP/1.1 505 HTTP Version Not Supported\r\n" +
                         getCurrentHttpDate() + "Server: " + "webserv\r\n\r\n";
  // this->_response = response;
  return (response);
}
