#ifndef Client_HPP
#define Client_HPP
#include <string>

#include "Socket.hpp"

class Client {
  public:
    enum class State {
      CONNECTED,
      NICK_RECEIVED,
      USER_RECEIVED,
      REGISTERED,
    };
    Client(Socket *soc);
    ~Client();

    /**
     * @brief This gets called when epoll() informs that the underlying FD has
     * data. Reads data from the underlying Socket, and appends it to the
     * socketBuffer. Calls checkBuffer to update status.
     */
    void readSocket();

    /**
     * @brief Extracts a message from the socketBuffer. Calls checkBuffer to
     * update status.
     * @return std::string The first full message contained in the socketBuffer.
     */
    std::string_view extractMessage();

    void eraseMessage();
    bool isRegistered();

    std::string getNickname();
    State       getState();

    void setNickname(std::string const &nick);

    void        fakeAppendToBuffer(std::string const &input);
    std::string fakeOutgoingBuffer();
    bool        hasMessage();
    bool        isOpen();

  private:
    Socket     *_socket;
    std::string _socketBuffer;
    bool        _hasMessage;
    bool        _isOpen;
    State       _state;
    std::string _nick;
    std::string _name;
    std::string _host;

    /**
     * @brief Checks if the socketBuffer contains a full message terminated by
     * "\r\n"
     * @return bool Is there a full message in buffer
     */
    bool checkBuffer();
};

#endif
