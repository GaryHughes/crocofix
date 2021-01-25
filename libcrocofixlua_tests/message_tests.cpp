#include <catch.hpp>
#include <libcrocofix/message.hpp>

using namespace crocofix;

#include "lua_fixture.hpp"

TEST_CASE_METHOD(crocofix::lua_fixture, "passing a message to an empty lua function does not modify the message") 
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    test(message);

    std::array<char, 1024> buffer;
    auto result = message.encode(gsl::span(buffer.data(), buffer.size()));
    REQUIRE(result > 0);
    auto actual = std::string_view(buffer.data(), result);
    REQUIRE(actual.compare(0, std::string::npos, expected) == 0);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "reset a message")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    message:reset()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto res = test(message);

    std::array<char, 1024> buffer;
    auto result = message.encode(gsl::span(buffer.data(), buffer.size()));
    REQUIRE(result == 0);
}

