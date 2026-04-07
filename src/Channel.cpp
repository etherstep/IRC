#include "Channel.hpp"

#include <stdexcept>

Channel::Channel(const Server &server, const Client &client,
                 const std::string &name)
    : _server(server), _name(name) {
  auto creator = std::make_unique<User>(client);
  creator->addOperatorPrivilege();
  _users.try_emplace(client.getNickname(), std::move(creator));
}

Channel::~Channel() {}

// INFO: Getters and setters:
const std::string &Channel::getName(void) const {
  return (_name);
}

void Channel::setName(const std::string &name) {
  _name = name;
}

const std::string &Channel::getTopic(void) const {
  return (_topic);
}

void Channel::setTopic(const std::string &topic) {
  _topic = topic;
}

void Channel::setUserLimit(const unsigned int limit) {
  if (_users.size() > limit)
    // FIXME: This is here for debugging purposes. How to handle this edge case?
    throw std::runtime_error(
        "Channel already has more users than the limit is");
  _userLimit = limit;
}

unsigned int Channel::getUserLimit(void) const {
  return (_userLimit);
}

unsigned int Channel::getUserCount(void) const {
  return (_users.size());
}

// INFO: Utilities:
Channel::User &Channel::addUser(const Client &client) {
  for (const auto &[key, value] : _users) {
    if (value->getClient() == &client)
      // FIXME:: Inform operator that User already exists?
      return (*value);
  }
  if (_users.size() < _userLimit) {
    _users.try_emplace(client.getNickname(), std::make_unique<User>(client));
    return (*_users.at(client.getNickname()));
  }
}

std::optional<std::reference_wrapper<Channel::User>> Channel::findUser(
    const std::string &nickname) {
  for (auto &[key, value] : _users) {
    if (value->getNickName() == nickname) {
      return std::ref(*value);
    }
  }
  return std::nullopt;
}

void Channel::resetFlags(void) {
  _channelFlags = 0;
}

void Channel::toggleFlag(const ChannelFlag flag) {
  _channelFlags ^= static_cast<uint16_t>(flag);
}

bool Channel::isFlagOn(const ChannelFlag flag) {
  return (_channelFlags & static_cast<uint16_t>(flag));
}

// INFO: Operator commands:
void Channel::kickUser(Channel::User &target) {
  // FIXME: What else needs to be done when kicking? Notify Client or Server?

  auto it = _users.find(target.getNickName());
  if (it != _users.end())
    _users.erase(it);
}

void Channel::kickUser(const std::string nickname) {
  std::optional<std::reference_wrapper<User>> target = findUser(nickname);
  if (target) {
    kickUser(target.value().get());
  }
}

void Channel::inviteUser(const std::string &nickname) {
  (void)nickname;
}

// INFO: Channel::User:
Channel::User::User(const Client &client) : _client(&client) {}

Channel::User::User(const User &other)
    : _client(other._client), _isOperator(other._isOperator) {}

Channel::User &Channel::User::operator&(const User &other) {
  if (this == &other)
    return (*this);
  _client = other._client;
  _isOperator = other._isOperator;
  return (*this);
}

const Client *Channel::User::getClient(void) const {
  return (_client);
}

const std::string &Channel::User::getNickName(void) const {
  return (_client->getNickname());
}

void Channel::User::toggleOperatorPrivilege(void) {
  _isOperator = !_isOperator;
}

void Channel::User::addOperatorPrivilege(void) {
  _isOperator = true;
}

void Channel::User::removeOperatorPrivilege(void) {
  _isOperator = false;
}
