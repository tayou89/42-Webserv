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
    std::string serverName, std::map<std::string, std::string> responseHeader,
    std::string responseBody) {
  std::string response = "HTTP/1.1 200 OK\r\n" + getCurrentHttpDate() +
                         "Server: " + serverName + "\r\n";
  for (std::map<std::string, std::string>::iterator itr =
           responseHeader.begin();
       itr != responseHeader.end(); ++itr)
    response = response + itr->first + ": " + itr->second + "\r\n";
  // std::cout << "this is response header:\n" << response << "\n" << std::endl;
  response = response + responseBody;
  return (response);
}

std::string errorResponse::create204Response(Config &conf) {
  std::string response = "HTTP/1.1 204 No Content\r\n" + getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(204).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create302Response(Config &conf, Location loc) {
  std::string response = "HTTP/1.1 302 Found\r\n" + getCurrentHttpDate() +
                         "Content-Length: 0\r\n"
                         "Server: " +
                         conf.getServerName() + "\r\n";
  std::string location = "Location: " + loc.getReturn() + "\r\n\r\n";
  response += location;
  return (response);
}

std::string errorResponse::create400Response(Config &conf) {
  std::string response = "HTTP/1.1 400 Bad Request\r\n" + getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(400).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create403Response(Config &conf) {
  std::string response = "HTTP/1.1 403 Forbidden\r\n" + getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  // this->_response = response;
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(403).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create404Response(Config &conf) {
  std::string response = "HTTP/1.1 404 Not Found\r\n" + getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  // this->_response = response;
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(404).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create405Response(Config &conf) {
  std::string response = "HTTP/1.1 405 Method Not Allowed\r\n" +
                         getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(405).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create413Response(Config &conf) {
  std::string response = "HTTP/1.1 413 Request Entity Too Large\r\n" +
                         getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(413).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create414Response(Config &conf) {
  std::string response = "HTTP/1.1 414 URI Too Long\r\n" +
                         getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(414).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create500Response(Config &conf) {
  std::string response = "HTTP/1.1 500 Internal Server Error\r\n" +
                         getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(500).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}

std::string errorResponse::create505Response(Config &conf) {
  std::string response = "HTTP/1.1 505 HTTP Version Not Supported\r\n" +
                         getCurrentHttpDate() +
                         "Server: " + conf.getServerName() + "\r\n";
  std::string body;
  int readSize;
  int contentLength = 0;

  int fd = open(conf.getErrorPage(505).c_str(), O_RDONLY);
  if (fd == -1)
    return (response);
  char buf[1000]; // change this
  memset(&buf[0], 0, 1000);
  while ((readSize = read(fd, buf, 1000)) > 0) {
    std::string tmp_body(buf);
    body = body + buf;
    contentLength += readSize;
  }
  std::stringstream ss;
  ss << contentLength;
  response += "Content-Length: " + ss.str() + "\r\n\r\n";
  response = response + body;
  close(fd);
  return (response);
}
