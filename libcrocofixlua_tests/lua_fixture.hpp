#ifndef libcrocofixlua_tests_lua_fixture_hpp
#define libcrocofixlua_tests_lua_fixture_hpp

#include <libcrocofixlua/message_type.hpp>
#include <catch.hpp>
namespace crocofix
{
struct lua_fixture
{
    lua_fixture()
    {
        initialise_message_type(lua);
    }

    sol::state lua;

    crocofix::message parse(const std::string_view& text)
    {
        crocofix::message message;
        auto [consumed, complete] = message.decode(text);
        REQUIRE(complete);
        return message;
    }

    sol::protected_function_result execute(crocofix::message& message)
    {
        auto test = lua["test"];
        REQUIRE(test.valid());

        auto result = test(message);

        if (!result.valid()) {
            sol::error err = result;
            std::cerr << err.what() << std::endl;

        }

        return result;
    }


};

}

#endif