#include "Parser.hpp"

#include <algorithm>

#include "Channel.hpp"
#include "Server.hpp"
#include "Utils.hpp"

int32_t Parser::channelModeParse(const Command &cmd, Channel &channel) {
  bool               onOff = false;
  size_t             index = 2;
  std::string        onBuffer = "";
  std::string        offBuffer = "";
  std::string        newModesBuffer = "";
  const std::string &modestring = cmd.params[1];
  for (auto &c : modestring) {
    switch (c) {
      case '+':
        onOff = true;
        onBuffer += '+';
        continue;

      case '-':
        onOff = false;
        offBuffer += '-';
        continue;

      case 'i':
        if (!onOff)
          offBuffer += 'i';
        else
          onBuffer += 'i';
        channel.setMode(Channel::ChannelMode::INVITE_ONLY, onOff);
        continue;

      case 't':
        if (!onOff)
          offBuffer += 't';
        else
          onBuffer += 't';
        channel.setMode(Channel::ChannelMode::TOPIC_SET_BY_CHANOP_ONLY, onOff);
        continue;

      case 'k': {
        if (!onOff) {
          if (channel.isModeOn(Channel::ChannelMode::KEY_PROTECTED))
            offBuffer += 'k';
          channel.setMode(Channel::ChannelMode::KEY_PROTECTED, false);
          channel.setKey("");
          continue;
        }
        if (index >= cmd.params.size())
          continue;
        channel.setMode(Channel::ChannelMode::KEY_PROTECTED, true);
        channel.setKey(cmd.params[index]);
        if (onOff)
          onBuffer += 'k';
        index++;
        continue;
      }

      case 'o': {
        if (index >= cmd.params.size())
          continue;
        OptionalUser user = channel.findUser(cmd.params[index]);
        // FIXME: handle missing user some way?
        if (!user)
          continue;
        user->get().toggleOperatorPrivilege();
        if (!onOff)
          offBuffer += 'o';
        else
          onBuffer += 'o';
        index++;
        continue;
      }

      case 'l': {
        if (!onOff) {
          offBuffer += 'l';
          channel.setUserLimit(UINT32_MAX);
          channel.setMode(Channel::ChannelMode::LIMITED_USER_COUNT, false);
          continue;
        }
        if (index >= cmd.params.size())
          continue;
        if (!std::ranges::all_of(cmd.params[index], ::isdigit))
          continue;
        try {
          channel.setUserLimit(
              static_cast<uint32_t>(std::stoul(cmd.params[index])));
          channel.setMode(Channel::ChannelMode::LIMITED_USER_COUNT, true);
        } catch (...) {}
        onBuffer += 'l';
        index++;
        continue;
      }

      default:
        return index;
    }
  }
  onBuffer.erase(std::unique(onBuffer.begin(), onBuffer.end()), onBuffer.end());
  offBuffer.erase(std::unique(offBuffer.begin(), offBuffer.end()),
                  offBuffer.end());
  newModesBuffer += onBuffer + offBuffer;
  channel.setNewModes(newModesBuffer);
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
