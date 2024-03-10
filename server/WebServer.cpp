#include "../include/WebServer.hpp"
#include <iostream> // test

WebServer::WebServer(Config conf) : _config(conf) {
  _listenSocket = socket(AF_INET, SOCK_STREAM, 0);

  if (_listenSocket == -1)
    exit(1); // socket error

  /* set socket option */
  int optval = 1;
  if (setsockopt(_listenSocket, SOL_SOCKET, SO_REUSEADDR, &optval,
                 sizeof(optval)) < 0) {
    close(_listenSocket); // 소켓 닫기
    exit(1);
  }

  memset(&_serverAddress, 0, sizeof(_serverAddress));
  _serverAddress.sin_family = AF_INET;
  _serverAddress.sin_port = htons(conf.getPortNumber());
  inet_pton(AF_INET, conf.getIPAddress().c_str(),
            &_serverAddress.sin_addr.s_addr);

  if (bind(_listenSocket, (struct sockaddr *)&_serverAddress,
           sizeof(_serverAddress)) == -1) {
    std::cout << "bind error\n";
    exit(1); // bind error
  }

  if (listen(_listenSocket, 64) == -1) // siege test 수행 후 backlog 사이즈 변경
    exit(1);                           // listen error
  fcntl(_listenSocket, F_SETFL, O_NONBLOCK, FD_CLOEXEC);
}

WebServer::~WebServer() {}

int WebServer::getListenSocket() const { return (_listenSocket); }

Config WebServer::getConfig() const { return (_config); }

// struct kevent WebServer::setEvent(uintptr_t ident, int16_t filter,
//                                   uint16_t flags, uint32_t fflags,
//                                   intptr_t data, void *udata) {
//   struct kevent temp;

//   EV_SET(&temp, ident, filter, flags, fflags, data, udata);
//   return (temp);
// }
