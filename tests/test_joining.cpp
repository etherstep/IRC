#include "Client.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "catch.hpp"

TEST_CASE("New User Registration Flow", "[routing][registration]") {
  Server server(6667, 1024, "usva");
  Client fake_client(Socket::makeClientSocket(-1));
  server.addClient(&fake_client);

  SECTION("Valid NICK and USER registers the client") {
    // Step 1: Send NICK
    fake_client.fakeAppendToBuffer("NICK myCoolUser\r\n");
    server.processMessage(&fake_client);

    // Assert state changed, but not fully registered
    REQUIRE(fake_client.getNickname() == "myCoolUser");
    REQUIRE(fake_client.getState() == Client::State::NICK_RECEIVED);

    // Step 2: Send USER
    fake_client.fakeAppendToBuffer("USER myuser 0 * :Real Name\r\n");
    server.processMessage(&fake_client);

    // Assert fully registered and 001 Welcome was queued to send
    REQUIRE(fake_client.getState() == Client::State::REGISTERED);
    REQUIRE(fake_client.fakeOutgoingBuffer().find("001 myCoolUser") !=
            std::string::npos);
  }

  SECTION("USER before NICK does not complete registration") {
    // Send USER first
    fake_client.fakeAppendToBuffer("USER myuser 0 * :Real Name\r\n");
    server.processMessage(&fake_client);

    // Should not be registered yet
    REQUIRE(fake_client.getState() != Client::State::REGISTERED);
    // Should not have received the welcome message
    REQUIRE(fake_client.fakeOutgoingBuffer().empty() == true);

    // Now send NICK
    fake_client.fakeAppendToBuffer("NICK myCoolUser\r\n");
    server.processMessage(&fake_client);

    // NOW it should be registered
    REQUIRE(fake_client.getState() == Client::State::REGISTERED);
  }

  SECTION("NICK missing parameters returns 431 ERR_NONICKNAMEGIVEN") {
    fake_client.fakeAppendToBuffer("NICK ");
    server.processMessage(&fake_client);  // No nickname provided

    REQUIRE(fake_client.getState() != Client::State::NICK_RECEIVED);
    REQUIRE(fake_client.fakeOutgoingBuffer().find("431") != std::string::npos);
  }
}
