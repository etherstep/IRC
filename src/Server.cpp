#include "Server.hpp"

#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#include <cstdint>

#include "Socket.hpp"

Server::Server(const int32_t port, const uint32_t backlogSize,
               const std::string &pwd)
    : _port(port), _backlogSize(backlogSize), _pwd(pwd) {
  int sendBufSize = SNDBUF_SIZE;
  int receiveBufSize = RCVBUF_SIZE;

  _listener = Socket::makeListeningSocket(port);
  if (_listener == nullptr)
    throw std::runtime_error("Couldn't create listening socket");

  if (setsockopt(_listener->getFD(), SOL_SOCKET, SO_RCVBUF, &receiveBufSize,
                 sizeof(receiveBufSize)) < 0)
    throw std::runtime_error(
        "Failed to set server listen socket receive buffer size");
  if (setsockopt(_listener->getFD(), SOL_SOCKET, SO_SNDBUF, &sendBufSize,
                 sizeof(sendBufSize)) < 0)
    throw std::runtime_error(
        "Failed to set server listen socket send buffer size");
}

void Server::start(void) {
  _addressLen = sizeof(_address);
  if ((_epollfd = epoll_create(_backlogSize)) < 0)
    throw std::runtime_error("Failed to set server");
  _ev.events = EPOLLIN;
  _ev.data.fd = _listener->getFD();
  if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, _listener->getFD(), &_ev) < 0)
    throw std::runtime_error("Failed to start polling on listening socket");

  _events = new struct epoll_event[_backlogSize];
}

void Server::poll(void) {
  while (true) {
    std::cout << "Polling for new connections\n";
    _nfds = epoll_wait(_epollfd, _events, _backlogSize, 100);
    for (int i = 0; i < _nfds; ++i) {
      if (_events[i].data.fd == _listener->getFD()) {
        int32_t connectionFD = accept(
            _listener->getFD(), (struct sockaddr *)&_address, &_addressLen);
        _clients.push_back(Socket::makeClientSocket(connectionFD));
      }
    }
    std::cout << _clients.size() << std::endl;
  }
}

Server::~Server(void) {
  if (_epollfd != -1)
    close(_epollfd);
  if (_events)
    delete[] _events;
}
