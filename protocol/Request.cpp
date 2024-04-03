#include "../include/Request.hpp"

Request::Request() {}

Request::Request(Config &conf) : _config(conf) {}

Request::~Request() {}

Request::Request(const Request &copy) {
  this->_request = copy._request;
  this->_requestHeader = copy._requestHeader;
  this->_requestBody = copy._requestBody;
  this->_requestMethod = copy._requestMethod;
  this->_requestURI = copy._requestURI;
  this->_queryString = copy._queryString;
  this->_location = copy._location;
}

Request &Request::operator=(const Request &copy) {
  this->_request = copy._request;
  this->_requestHeader = copy._requestHeader;
  this->_requestBody = copy._requestBody;
  this->_requestMethod = copy._requestMethod;
  this->_requestURI = copy._requestURI;
  this->_queryString = copy._queryString;
  this->_location = copy._location;
  return (*this);
}

void Request::setRequest(std::string packet) {
  std::string firstLine = packet.substr(0, packet.find("\r\n"));
  this->readStartLine(firstLine);
  std::string header = packet.substr(packet.find("\r\n") + 2);
  this->readHeader(header);
}

void Request::readStartLine(std::string startLine) {
  std::string requestHTTPVersion;
  std::string::size_type pos1 = startLine.find(" ");
  std::string::size_type pos2 = startLine.find(" ", pos1 + 1);

  if (pos1 == std::string::npos || pos2 == std::string::npos) {
    throw(this->_errorResponse.create400Response(this->_config));
  }
  this->_requestMethod = startLine.substr(0, pos1);
  this->_requestURI = startLine.substr(pos1 + 1, pos2 - pos1 - 1);
  this->_requestHTTPVersion =
      startLine.substr(pos2 + 1, startLine.size() - pos2);
}

void Request::readHeader(std::string header) {
  std::size_t pos = 0;
  std::size_t index = 0;
  std::string tmp;

  while (1) {
    index = header.find("\r\n", pos + 1);
    if (index == std::string::npos)
      break;
    tmp = header.substr(pos, index - pos);
    std::string key = splitBefore(tmp, ":");
    std::string value = splitAfter(tmp, ":");
    this->_requestHeader.insert(std::make_pair(key, value));
    if (header.find("\r\n", index + 1) == index + 1)
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
  if (_requestHeader.find("Transfer-Encoding") != _requestHeader.end() &&
      _requestHeader.find("Transfer-Encoding")->second == "chunked")
    return (CHUNKED_READ);
  else if (_requestHeader.find("Content-Length") != _requestHeader.end() &&
           _requestHeader.find("Content-Length")->second != "0") {
    return (BODY_READ);
  } else
    return (WRITE);
}

void Request::checkValidHeader(std::string key, std::string value) {
  if (key.empty() || value.empty()) {
    throw(this->_errorResponse.create400Response(this->_config));
  }
}

void Request::readBody(std::vector<unsigned char> body) {
  this->_requestBody = body;
}

std::string Request::getRequestMethod() const { return (this->_requestMethod); }

std::string Request::getRequestURI() const { return (this->_requestURI); }

std::vector<unsigned char> Request::getRequestBody() const {
  return (this->_requestBody);
}

std::map<std::string, std::string> Request::getRequestHeader() const {
  return (this->_requestHeader);
}

std::string Request::getRequestHeader(std::string key) const {
  if (this->_requestHeader.find(key) != this->_requestHeader.end())
    return (this->_requestHeader.find(key)->second);
  else
    return ("");
}

std::string Request::getQueryStirng() const { return (_queryString); }

std::string Request::getProtocolVersion() const {
  return (_requestHTTPVersion);
}

Location Request::getLocation() const { return (_location); }

void Request::setRequestHeader(std::string key, std::string value) {
  this->_requestHeader.insert(std::make_pair(key, value));
}

bool Request::extensionURI(std::string location) const {
  std::vector<std::string> url = splitString(_requestURI, '/');
  std::vector<std::string>::iterator urlIter = url.begin();
  std::string extension = location.substr(location.find("."));

  if (extension.size() == 0)
    return (false);
  for (; urlIter != url.end(); urlIter++) {
    if ((*urlIter).size() > extension.size() &&
        (*urlIter).compare((*urlIter).size() - extension.size(),
                           extension.length(), extension) == 0)
      return (true);
  }
  return (false);
}

std::size_t Request::generalURI(std::string location) const {
  std::vector<std::string> loc = splitString(location, '/');
  std::vector<std::string> url = splitString(_requestURI, '/');
  std::vector<std::string>::iterator locIter = loc.begin();
  std::vector<std::string>::iterator urlIter = url.begin();
  std::size_t score = 0;

  /* general location */
  for (; locIter != loc.end() && urlIter != url.end(); locIter++, urlIter++) {
    if (*locIter == *urlIter)
      score++;
  }
  if (score == loc.size())
    return (score);
  else
    return (0);
}

std::string Request::combinePATH(Location target, std::size_t rate) const {
  std::vector<std::string> split = splitString(_requestURI, '/');
  std::stringstream ss;
  std::string path;

  for (; rate < split.size(); rate++) {
    ss << "/" << split[rate];
  }
  path = target.getRootDirectory() + ss.str();

  std::vector<std::string> index = target.getIndexes();
  std::vector<std::string>::iterator iter = index.begin();
  std::string tmpPath;
  for (; iter != index.end(); iter++) {
    if ((*iter)[0] == '/')
      tmpPath = path + *iter;
    else
      tmpPath = path + "/" + *iter;
    if (access(tmpPath.c_str(), F_OK) == 0)
      return (tmpPath);
  }
  return (path);
}

void Request::convertURI() {
  std::map<std::string, Location> locationMap = _config.getLocationMap();
  std::map<std::string, Location>::iterator iter = locationMap.begin();
  Location *target = NULL;
  int rate = 0;
  int tmp = 0;

  replace(_requestURI, "%20", " ");
  /* separate query string */
  std::vector<std::string> query = splitString(_requestURI, '?');
  _requestURI = query[0];
  if (query.size() == 1)
    _queryString.append("");
  else
    _queryString = query[1];

  /* calculate similarity */
  for (; iter != locationMap.end(); iter++) {
    if (*(iter->first.begin()) == '~' && *(iter->first.begin() + 1) == '.') {
      if (extensionURI(iter->first)) {
        _location = iter->second;
        return;
      }
    } else
      tmp = generalURI(iter->first);
    if (tmp > rate) {
      rate = tmp;
      target = &iter->second;
    }
  }

  /* check default location */
  if (rate == 0 && locationMap.find("/") != locationMap.end())
    target = &locationMap["/"];

  /* convert URI to real path */
  if (target) {
    _location = *target;
  } else {
    target = &_config;
    _location = _config;
  }
  _requestURI = combinePATH(*target, rate);
}

void Request::eraseRequestBody(int start, int end) {
  if (start < 0 || end < 0)
    return;
  if (start > end)
    return;
  if (start + end > static_cast<int>(_requestBody.size()))
    end = _requestBody.size() - start;
  _requestBody.erase(_requestBody.begin() + start, _requestBody.begin() + end);
}

void Request::initRequest() {
  _request.clear();
  _requestHeader.clear();
  _requestBody.clear();
  _requestMethod.clear();
  _requestURI.clear();
  _queryString.clear();
  _location = Location();
}

void Request::checkRequestMethod() {
  std::vector<std::string> acceptedMethods = _location.getAcceptedMethods();
  std::vector<std::string>::iterator iter = acceptedMethods.begin();

  if (!acceptedMethods.empty()) {
    for (; iter != acceptedMethods.end(); iter++) {
      if (*iter == _requestMethod)
        break;
    }
    if (iter == acceptedMethods.end())
      throw _errorResponse.create405Response(_config);
  }
}

void Request::checkRequestValidity() {
  // if it is not method sp URI sp HTTP-Version, create 400 response
  if (this->_requestMethod.size() == 0 || this->_requestURI.size() == 0 ||
      this->_requestHTTPVersion.size() == 0) {
    throw(this->_errorResponse.create400Response(this->_config));
  }

  // if the requestURI is longer than large_client_header_buffers, create 414
  // response
  if (this->_requestURI.size() >
      static_cast<unsigned long>(_config.getClientHeaderMax()))
    throw(this->_errorResponse.create414Response(this->_config));

  // if the HTTP-Version is not HTTP/1.1, create 505 response, else create 400
  // response
  if (_requestHTTPVersion != "HTTP/1.1") {
    if (_requestHTTPVersion == "HTTP/0.9" ||
        _requestHTTPVersion == "HTTP/1.0" ||
        _requestHTTPVersion == "HTTP/2.0" || _requestHTTPVersion == "HTTP/3.0")
      throw(this->_errorResponse.create505Response(this->_config));
    else {
      throw(this->_errorResponse.create400Response(this->_config));
    }
  }

  // check request header validity & content-Length
  std::map<std::string, std::string>::iterator itr = _requestHeader.begin();
  for (; itr != _requestHeader.end(); ++itr) {
    if (itr->first.empty() || itr->second.empty())
      throw(this->_errorResponse.create400Response(_config));
    if (itr->first == "Content-Length") {
      if (atol(itr->second.c_str()) >
          static_cast<long>(_config.getClientBodyMax()))
        throw(this->_errorResponse.create413Response(_config));
    }
  }

  // read body
  if (_requestHeader.find("Content-Length") != _requestHeader.end()) {
    if (_requestBody.size() >
        static_cast<unsigned long>(
            std::atol(this->_requestHeader["Content-Length"].c_str())))
      throw(this->_errorResponse.create413Response(this->_config));
    else if (_requestBody.size() <
             static_cast<unsigned long>(
                 std::atol(this->_requestHeader["Content-Length"].c_str())))
      throw(this->_errorResponse.create400Response(this->_config));
  }
}
