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

    REQUIRE(message.to_string() == expected);
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

    test(message);

    REQUIRE(message.to_string().empty());
}

TEST_CASE_METHOD(crocofix::lua_fixture, "query field count")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:fields():size()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<int>() == 18);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test MsgType")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:MsgType()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<std::string>() == "D");
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test MsgSeqNum")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:MsgSeqNum()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<int>() == 2752);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test PossDupFlag")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:PossDupFlag()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test GapFillFlag")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:GapFillFlag()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test ResetSeqNumFlag")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:ResetSeqNumFlag()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test is_admin")
{
    std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    crocofix::message message;
    auto [consumed, complete] = message.decode(expected);
    REQUIRE(complete);

    lua.safe_script(
        "function test(message)\n"
        "    return message:is_admin()\n"
        "end\n"
    );

    auto test = lua["test"];
    REQUIRE(test.valid());

    auto result = test(message);

    if (!result.valid()) {
        sol::error err = result;
		std::cerr << err.what() << std::endl;
    }

    REQUIRE(result.get<bool>() == false);
}

