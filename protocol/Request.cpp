#include "../include/Request.hpp"

#define LARGE_CLIENT_HEADER_BUFFERS 1000
#define MAX_BODY_SIZE 1000

Request::Request() {}

Request::Request(Config conf) : _config(conf) {}

Request::~Request() {}

Request::Request(const Request &copy) {
  this->_request = copy._request;
  this->_requestHeader = copy._requestHeader;
  this->_requestBody = copy._requestBody;
  this->_requestMethod = copy._requestMethod;
  this->_requestURI = copy._requestURI;
}

Request &Request::operator=(const Request &copy) {
  this->_request = copy._request;
  this->_requestHeader = copy._requestHeader;
  this->_requestBody = copy._requestBody;
  this->_requestMethod = copy._requestMethod;
  this->_requestURI = copy._requestURI;
  return (*this);
}

void Request::setRequest(std::string packet) {
  //   if (packet.find("\r\n\r\n") == std::string::npos ||
  //       packet.find("\r\n") == std::string::npos)
  //     throw(this->_errorResponse.create400Response());
  std::string firstLine = packet.substr(0, packet.find("\r\n"));
  this->readStartLine(firstLine, LARGE_CLIENT_HEADER_BUFFERS);

  std::string header = packet.substr(packet.find("\r\n") + 2);
  // packet.find("\r\n\r\n") - packet.find("\r\n") - 2)/
  this->readHeader(header);

  this->checkContentLength(MAX_BODY_SIZE);
}

void Request::readStartLine(std::string startLine,
                            int large_client_header_buffers) {
  std::string requestHTTPVersion;
  std::string::size_type pos1 = startLine.find(" ");
  std::string::size_type pos2 = startLine.find(" ", pos1 + 1);

  if (pos1 == std::string::npos || pos2 == std::string::npos) {
    throw(this->_errorResponse.create400Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));
  }
  this->_requestMethod = startLine.substr(0, pos1);
  this->_requestURI = startLine.substr(pos1 + 1, pos2 - pos1 - 1);
  requestHTTPVersion = startLine.substr(pos2 + 1, startLine.size() - pos2);

  // if it is not method sp URI sp HTTP-Version, create 400 response
  if (this->_requestMethod.size() == 0 || this->_requestURI.size() == 0 ||
      requestHTTPVersion.size() == 0) {
    throw(this->_errorResponse.create400Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));
  }

  // if it is not GET, HEAD, DELETE, OPTIONS, POST, PUT, TRACE, create 400
  // response
  if (!(this->_requestMethod == "GET" || this->_requestMethod == "HEAD" ||
        this->_requestMethod == "DELETE" || this->_requestMethod == "OPTIONS" ||
        this->_requestMethod == "POST" || this->_requestMethod == "PUT" ||
        this->_requestMethod == "TRACE")) {
    throw(this->_errorResponse.create400Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));
  }

  // if the requestURI is longer than large_client_header_buffers, create 414
  // response
  if (this->_requestURI.size() >
      static_cast<unsigned long>(large_client_header_buffers))
    throw(this->_errorResponse.create414Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));

  // if the HTTP-Version is not HTTP/1.1, create 505 response, else create 400
  // response
  if (requestHTTPVersion != "HTTP/1.1") {
    if (requestHTTPVersion == "HTTP/0.9" || requestHTTPVersion == "HTTP/1.0" ||
        requestHTTPVersion == "HTTP/2.0" || requestHTTPVersion == "HTTP/3.0")
      throw(this->_errorResponse.create505Response(
          this->_config.getLocation(this->getRequestURI()),
          this->_config.getServerName()));
    else {
      throw(this->_errorResponse.create400Response(
          this->_config.getLocation(this->getRequestURI()),
          this->_config.getServerName()));
    }
  }
}

void Request::readHeader(std::string header) {
  size_t pos = 0;
  size_t index = 0;
  // size_t colon_index = 0;
  std::string tmp;

  while (1) {
    index = header.find("\r\n", pos + 2);
    if (index == std::string::npos)
      break;
    tmp = header.substr(pos + 2, index - pos - 2);
    std::string key = splitBeforeColon(tmp);
    std::string value = splitAfterColon(tmp);
    this->_requestHeader.insert(std::make_pair(key, value));
    if (header.find("\r\n", index + 2) == index + 2)
      break;
    pos = index + 2;
  }
  for (std::map<std::string, std::string>::iterator it =
           this->_requestHeader.begin();
       it != this->_requestHeader.end(); it++) {
    this->checkValidHeader(it->first, it->second);
  }
}

int Request::checkBodyExistence() const {
  if (_requestHeader.find("Transfer-encoding") != _requestHeader.end() &&
      _requestHeader.find("Transfer-encoding")->second == "chunked")
    return (CHUNKED_READ);
  else if (_requestHeader.find("Content-length") != _requestHeader.end() &&
           _requestHeader.find("Content-length")->second != "0")
    return (BODY_READ);
  else
    return (WRITE);
}

void Request::checkValidHeader(std::string key, std::string value) {
  if (key.empty() || value.empty()) {
    throw(this->_errorResponse.create400Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));
  }

  // check validity of mandatory headers??
}

void Request::checkContentLength(int client_max_body_size) {
  if (this->_requestHeader.find("Content-Length") !=
      this->_requestHeader.end()) {
    if (this->_requestHeader["Content-Length"].size() > 10) {
      throw(this->_errorResponse.create400Response(
          this->_config.getLocation(this->getRequestURI()),
          this->_config.getServerName()));
    }
    if (std::stoi(this->_requestHeader["Content-Length"]) >
        client_max_body_size)
      throw(this->_errorResponse.create413Response(
          this->_config.getLocation(this->getRequestURI()),
          this->_config.getServerName()));
  }
}

void Request::readBody(std::string body) {
  if (body.size() > static_cast<unsigned long>(
                        std::stol(this->_requestHeader["Content-Length"])))
    throw(this->_errorResponse.create413Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));
  else if (body.size() < static_cast<unsigned long>(
                             std::stol(this->_requestHeader["Content-Length"])))
    throw(this->_errorResponse.create400Response(
        this->_config.getLocation(this->getRequestURI()),
        this->_config.getServerName()));
  else
    this->_requestBody = body;
}

std::string Request::getRequestMethod() const { return (this->_requestMethod); }

std::string Request::getRequestURI() const { return (this->_requestURI); }

std::string Request::getRequestBody() const { return (this->_requestBody); }

std::map<std::string, std::string> Request::getRequestHeader() const {
  return (this->_requestHeader);
}

std::string Request::getRequestHeader(std::string key) const {
  if (this->_requestHeader.find(key) != this->_requestHeader.end())
    return (this->_requestHeader.find(key)->second);
  else
    return ("");
}

void Request::setRequestHeader(std::string key, std::string value) {
  this->_requestHeader.insert(std::make_pair(key, value));
}

void Request::convertURI() {
  //   std::map<std::string, Location> location = _config.getLocationMap();
  //   std::map<std::string, Location>::iterator target =
  //   location.find(_requestURI);
  Location target = _config.getLocation(_requestURI);

  /* URI가 존재하지 않으면 404 not found throw */
  //   if (target == location.end())
  // 	throw _errorResponse.create404Response();

  //   _requestURI = target->second.getIndexFile().getPath();
  std::cout << target.getRootDirectory() << "-> ROOT\n";
  std::cout << target.getIndexFile().getPath() << "-> File PATH\n";
  _requestURI = target.getRootDirectory() + "/" + "index.html";
}
