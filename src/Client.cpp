#include "Client.hpp"

#include <sys/socket.h>

#include <cerrno>
#include <stdexcept>
#include <string>
#include <string_view>

#include "Logger.hpp"
#include "Server.hpp"
#include "Socket.hpp"

Client::Client(Socket *soc)
    : _socket(soc),
      _socketBuffer(""),
      _hasMessage(false),
      _isOpen(true),
      _state(State::CONNECTED) {};

Client::~Client() {
  if (_socket != nullptr)
    delete _socket;
};

bool Client::checkBuffer() {
  return false;
}

void Client::fakeAppendToBuffer(std::string const &input) {
  _socketBuffer.append(input);
}

void Client::eraseMessage() {
  auto end = _socketBuffer.find("\r\n");
  if (end != std::string::npos)
    _socketBuffer.erase(end + 2);
}

void Client::readSocket() {
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

std::string_view Client::extractMessage() {
  auto stoppingPoint = _socketBuffer.find("\r\n");
  if (stoppingPoint != std::string::npos) {
    std::string_view msg(_socketBuffer.data(), stoppingPoint + 2);
    return msg;
  } else {
    return "";
  }
}

bool Client::hasMessage() {
  return _hasMessage;
}

bool Client::isOpen() {
  return _isOpen;
}
