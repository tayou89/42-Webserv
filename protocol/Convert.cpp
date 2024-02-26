#include "../include/Convert.hpp"
#include "../include/Protocol.hpp"

#define LARGE_CLIENT_HEADER_BUFFERS 1000
#define MAX_BODY_SIZE 1000

Convert::Convert() {}

Convert::~Convert() {}

Convert::Convert(const Convert &) {}

Convert &Convert::operator=(const Convert &) { return (*this); }

void Convert::setEnvp(char **envp) { this->envp = envp; }

void Convert::getRequest(int fd) {
  char buf[LARGE_CLIENT_HEADER_BUFFERS + MAX_BODY_SIZE + 1];
  int readSize = read(fd, buf, LARGE_CLIENT_HEADER_BUFFERS + MAX_BODY_SIZE);
  if (readSize == -1) {
    std::cout << "read error" << std::endl;
    throw("");
  }
  buf[readSize] = '\0';
  std::string packet(buf);

  // check if \r\n\r\n and \r\n exists
  if (packet.find("\\r\\n\\r\\n") == std::string::npos ||
      packet.find("\\r\\n") == std::string::npos)
    throw(this->_protocol.create400Response());
  std::string firstLine = packet.substr(0, packet.find("\\r\\n"));
  this->_protocol.readStartLine(firstLine, LARGE_CLIENT_HEADER_BUFFERS);

  std::string header =
      packet.substr(packet.find("\\r\\n") + 2,
                    packet.find("\\r\\n\\r\\n") - packet.find("\\r\\n") - 2);
  this->_protocol.readHeader(header);

  this->_protocol.checkContentLength(MAX_BODY_SIZE);

  // if (chunked == true)
  // 	std::string body = packet.substr(packet.find("\r\n\r\n") + 4,
  // getcontentLength()); else
  // {
  // 	//read more chunked data
  // }

  // for now, let's say that the body is well added and ready to execute.
  std::string body = packet.substr(packet.find("\\r\\n\\r\\n") + 8);
  std::map<std::string, std::string> pro = this->_protocol.getRequestHeader();
  if (pro.find("Content-Length") != pro.end())
    this->_protocol.readBody(body);
}

void Convert::checkValidity() {
  DIR *dir = opendir(this->_protocol.getRequestURI().c_str());
  if (dir != NULL) {
    // // 1. check index directive in conf file
    // if (index_directive == true)
    // {
    // 	//find index files in order, then put it in the response packet
    // }

    // // 2. check if autoindex is enabled
    // if (autoindex == true)
    // {
    // 	//read all files in the directory and put it in the response packet
    // 	struct dirent *ent;
    // 	std::string filelist = "";
    // 	while ((ent = readdir(dir)) != NULL)
    // 		filelist = filelist + ent->d_name + "\n";
    // 	this->_protocol.setResponseBody(filelist);
    // 	this->_protocol.setResponseHeader("Content-Length",
    // std::to_string(filelist.size()));
    // 	// this->_protocol.setResponseHeader("Transfer-Encoding", "chunked");
    // 	this->_protocol.setResponseHeader("Content-Type", "directory Listing");
    // //need to change
    // this->_protocol.setResponseHeader("Last-Modified", getCurrentHttpDate());
    // this->_protocol.create200Response();
    // }
    closedir(dir);
  } else { // if URI is a file
    // 1. check if the method is allowed or executable
    // -> going to check it in executeMethod()
    // 2. check if authorization header exists
    if (this->_protocol.getRequestHeader("Authorization") != "") {
      // check if the user is authorized
      // if not, throw 401 response
    }
    this->executeMethod();
  }

  // check more...
}

void Convert::executeMethod() {
  if (this->_protocol.getRequestMethod() == "GET" ||
      this->_protocol.getRequestMethod() == "HEAD")
    GET_HEAD();
  else if (this->_protocol.getRequestMethod() == "POST")
    POST();
  else if (this->_protocol.getRequestMethod() == "DELETE")
    DELETE();
  else if (this->_protocol.getRequestMethod() == "PUT")
    PUT();
  else if (this->_protocol.getRequestMethod() == "OPTIONS")
    OPTIONS();
  else if (this->_protocol.getRequestMethod() == "TRACE")
    TRACE();
  else
    throw(this->_protocol.create405Response());
}

void Convert::sendResponse(int fd) {
  (void)fd;
  // print instead of sending packet
  std::cout << this->_protocol.getResponse() << std::endl;
}
