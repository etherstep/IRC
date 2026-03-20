#include "Connection.hpp"

#include <cerrno>
#include <stdexcept>

#include "Logger.hpp"
#include "Server.hpp"
#include "Socket.hpp"

Connection::Connection(Socket *soc)
    : _socket(soc), _socketBuffer(""), _hasMessage(false), _isOpen(true) {};

Connection::~Connection() {
  if (_socket != nullptr)
    delete _socket;
};

bool Connection::checkBuffer() {
  return false;
}

void Connection::readSocket() {
  char    buffer[RCVBUF_SIZE] = {};
  ssize_t bytesRead = _socket->receiveData(buffer, RCVBUF_SIZE);
  if (bytesRead > 0) {
    _socketBuffer.append(buffer, bytesRead);
    _hasMessage = checkBuffer();
  } else if (bytesRead == 0) {
    LOG << "Socket closing, fd: " << _socket->getFD();
    _isOpen = false;
  } else {
    if (!(errno == EAGAIN || errno == EWOULDBLOCK)) {
      throw std::runtime_error("Error reading socket");
    }
  }
}

std::string Connection::getMessage() {
  if (!_hasMessage)
    return "";
  std::string msg;
  auto        stoppingPoint = _socketBuffer.find("\r\n");
  if (stoppingPoint != std::string::npos) {
    msg = _socketBuffer.substr(0, stoppingPoint);
  } else {
    return msg;
  }
  _socketBuffer.erase(_socketBuffer.begin(),
                      _socketBuffer.begin() + stoppingPoint + 2);
  _hasMessage = checkBuffer();
  return msg;
}

bool Connection::hasMessage() {
  return _hasMessage;
}
