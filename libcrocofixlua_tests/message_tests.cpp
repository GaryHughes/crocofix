#include <catch.hpp>
#include <libcrocofix/message.hpp>

using namespace crocofix;

#include "lua_fixture.hpp"

TEST_CASE_METHOD(crocofix::lua_fixture, "passing a message to an empty lua function does not modify the message") 
{
    auto const * const expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    auto message = parse(expected);

    lua.safe_script(
        "function test(message)\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "reset a message")
{
    auto const * const expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    auto message = parse(expected);

    lua.safe_script(
        "function test(message)\n"
        "    message:reset()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(to_string(message).empty());
}

TEST_CASE_METHOD(crocofix::lua_fixture, "query field count")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");

    lua.safe_script(
        "function test(message)\n"
        "    return message:fields():size()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<int>() == 18);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test MsgType")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");

    lua.safe_script(
        "function test(message)\n"
        "    return message:MsgType()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<std::string>() == "D");
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test MsgSeqNum")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");

    lua.safe_script(
        "function test(message)\n"
        "    return message:MsgSeqNum()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<int>() == 2752);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test PossDupFlag")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");

    lua.safe_script(
        "function test(message)\n"
        "    return message:PossDupFlag()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test GapFillFlag")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");

    lua.safe_script(
        "function test(message)\n"
        "    return message:GapFillFlag()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test ResetSeqNumFlag")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
   
    lua.safe_script(
        "function test(message)\n"
        "    return message:ResetSeqNumFlag()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "test is_admin")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    return message:is_admin()\n"
        "end\n"
    );

    auto result = execute(message);

    REQUIRE(result.get<bool>() == false);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "try get existent field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    return message:fields():try_get(56)\n"
        "end\n"
    );

    auto result = execute(message);
    auto acceptor = result.get<std::optional<crocofix::field>>();

    REQUIRE(acceptor);
    if (!acceptor.has_value()) {
        REQUIRE(false);
        return;
    }
    REQUIRE(acceptor->value() == "ACCEPTOR");
}

TEST_CASE_METHOD(crocofix::lua_fixture, "try get non existent field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    return message:fields():try_get(72)\n"
        "end\n"
    );

    auto result = execute(message);
    auto acceptor = result.get<std::optional<crocofix::field>>();

    REQUIRE(!acceptor);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "set existing field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():set(56, \"BLAH\", set_operation.replace_first)\n"
        "end\n"
    );

    execute(message);

    auto const * const expected = "8=FIX.4.4\u00019=145\u000135=D\u000149=INITIATOR\u000156=BLAH\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=215\u0001";

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "set non existent field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():set(666, \"NEW\", set_operation.replace_first_or_append)\n"
        "end\n"
    );

    execute(message);

    auto const * const expected = "8=FIX.4.4\u00019=157\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u0001666=NEW\u000110=222\u0001";

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "append field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():set(666, \"NEW\", set_operation.append)\n"
        "end\n"
    );

    execute(message);

    auto const * const expected = "8=FIX.4.4\u00019=157\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u0001666=NEW\u000110=222\u0001";

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "remove first non existent field")
{
    auto const * const expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    auto message = parse(expected);
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():remove(666, remove_operation.remove_first)\n"
        "end\n"
    );

    execute(message);

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "remove all non existent field")
{
    auto const * const expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
    auto message = parse(expected);
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():remove(666, remove_operation.remove_all)\n"
        "end\n"
    );

    execute(message);

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "remove first existent field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():remove(70, remove_operation.remove_first)\n"
        "end\n"
    );

    execute(message);

    auto const * const expected = "8=FIX.4.4\u00019=143\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=004\u0001";

    REQUIRE(to_string(message) == expected);
}

TEST_CASE_METHOD(crocofix::lua_fixture, "remove all existent field")
{
    auto message = parse("8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000170=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001");
  
    lua.safe_script(
        "function test(message)\n"
        "    message:fields():remove(70, remove_operation.remove_all)\n"
        "end\n"
    );

    execute(message);

    auto const * const expected = "8=FIX.4.4\u00019=134\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000140=2\u000144=20\u000159=1\u000110=106\u0001";

    REQUIRE(to_string(message) == expected);
}
