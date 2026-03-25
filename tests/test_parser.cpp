#include "Parser.hpp"
#include "catch.hpp"

TEST_CASE("Parser: New User Handshake", "[parser]") {
  SECTION("Parses PASS command correctly") {
    auto result = Parser::parse("PASS secret_password");

    REQUIRE(result.has_value() == true);
    REQUIRE(result->command == "PASS");
    REQUIRE(result->params.size() == 1);
    REQUIRE(result->params[0] == "secret_password");
  }

  SECTION("Parses NICK command correctly") {
    auto result = Parser::parse("NICK U$vA");

    REQUIRE(result.has_value() == true);
    REQUIRE(result->command == "NICK");
    REQUIRE(result->params.size() == 1);
    REQUIRE(result->params[0] == "U$vA");
  }

  SECTION("Parses USER command and trailing colon correctly") {
    auto result = Parser::parse("USER guest 0 * :Suuri ja mahtava Usva");

    REQUIRE(result.has_value() == true);
    REQUIRE(result->command == "USER");
    REQUIRE(result->params.size() == 4);
    REQUIRE(result->params[0] == "guest");
    REQUIRE(result->params[1] == "0");
    REQUIRE(result->params[2] == "*");
    REQUIRE(result->params[3] == "Suuri ja mahtava Usva");
  }

  SECTION("Fails gracefully on malformed strings") {
    auto emptyResult = Parser::parse("");
    REQUIRE(emptyResult.has_value() == false);

    auto prefixOnlyResult = Parser::parse(":JustAPrefix");
    REQUIRE(prefixOnlyResult.has_value() == false);
  }
}
