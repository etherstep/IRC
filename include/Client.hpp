#pragma once
#include <pwd.h>
#include <unistd.h>

#include <iostream>
#include <unordered_map>
#include <vector>

#include "Socket.hpp"
class Client {
  public:
    Client() = default;
    Client(const Client &) = delete;
    Client &operator=(const Client &) = delete;
    Client(Client &&) = default;  // move constructor
    Client &operator=(Client &&) = default;
    ~Client() {};

    // const std::string &get_recv_buf() const {
    //   return _recv_buf;
    // }

    // void add_client(int fd) {
    //   clients.try_emplace(fd);
    // };

    const std::string &get_response() const {
      return _response;
    }

    void set_response(const std::string &msg) {
      _response = msg;
    }

    void append_recv_buf(const char buf[2048]) {
      _recv_buf += buf;
    }

    void set_recv_buf(const char buf[2048]) {
      _recv_buf = buf;
    }

    /**
     * @brief fetches the user's name in the system.
     */
    void set_nick() {
      struct passwd *pw = getpwuid(getuid());
      if (pw) {
        _nick = pw->pw_name;
      }
    }

    /**
     * @brief name
     *
     * @return
     */
    const std::string &get_nick() const {
      return _nick;
    }

    /**
     * @brief name
     *
     * @param socket
     */
    static void setClientSocket(Socket *socket) {
      _clientSocket = socket;
    }

    /**
     * @brief name
     *
     * @return
     */
    static Socket &getClientSocket() {
      return *_clientSocket;
    }

  private:
    static Socket *_clientSocket;
    std::string    _response;
    std::string    _nick;
    std::string    _username;
    std::string    _recv_buf;
    // bool                                   _registered = false;
};
