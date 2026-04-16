#include "Parser.hpp"

#include <charconv>
#include <cstdint>
#include <iostream>
#include <optional>
#include <sstream>

#include "Channel.hpp"
#include "Server.hpp"
#include "Utils.hpp"

int32_t Parser::channelModeParse(const Command &cmd, Channel &channel) {
  bool onOff = false;
  int  it = 2;
  for (auto &c : cmd.params[1]) {
    switch (c) {
      case '+':
        onOff = true;
        continue;
      case '-':
        onOff = false;
        continue;
      case 'i':
        channel.setMode(Channel::ChannelMode::INVITE_ONLY, onOff);
        continue;
      case 't':
        channel.setMode(Channel::ChannelMode::TOPIC_SET_BY_CHANOP_ONLY, onOff);
        continue;
      case 'k':
        channel.setMode(Channel::ChannelMode::KEY_PROTECTED, onOff);
        channel.setKey(cmd.params[it]);
        it++;
        continue;
      case 'o':
        channel.findUser(cmd.params[it])->get().toggleOperatorPrivilege();
        it++;
        continue;
      case 'l':
        channel.setMode(Channel::ChannelMode::LIMITED_USER_COUNT, onOff);
        try {
          channel.setUserLimit(
              static_cast<uint32_t>(std::stoul(cmd.params[it])));
        } catch (...) {}
        it++;
        continue;
      default:
        return it;
    }
  }
  return -1;
}

std::optional<Command> Parser::parse(std::string message) {
  Command cmd;

  if (message.empty())
    return std::nullopt;

  auto endpoint = message.find_first_of(' ');
  if (message[0] == ':') {
    if (endpoint == std::string::npos)
      return std::nullopt;
    cmd.prefix = std::string(message.substr(1, endpoint - 1));
    message.erase(0, endpoint + 1);
  }

  endpoint = message.find_first_of(' ');
  cmd.command = std::string(message.substr(0, endpoint));
  if (endpoint != std::string::npos) {
    message.erase(0, endpoint + 1);
  } else {
    return cmd;
  }

  while (!message.empty()) {
    if (message[0] == ' ') {
      message.erase(0, 1);
      continue;
    }
    endpoint = message.find_first_of(' ');
    if (message[0] == ':') {
      cmd.params.push_back(std::string(message.substr(1)));
      break;
    }
    cmd.params.push_back(std::string(message.substr(0, endpoint)));
    if (endpoint != std::string::npos) {
      message.erase(0, endpoint + 1);
    } else {
      break;
    }
  }
  return cmd;
}
