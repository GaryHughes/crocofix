#ifndef libcrocofixlua_tests_lua_fixture_hpp
#define libcrocofixlua_tests_lua_fixture_hpp

#include <libcrocofixlua/types.hpp>
#include <catch2/catch_all.hpp>

import crocofix.dictionary;

namespace crocofix
{
struct lua_fixture
{
    lua_fixture()
    {
        define_lua_types(lua);
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
            FAIL(err.what());
        }

        return result;
    }

    std::string to_string(crocofix::message& message, int options = encode_options::standard)
    {
        if (message.fields().empty()) {
            return std::string();
        }
        // Lots of tests append fields which will place them after the checksum. Encoding stops at the Checksum
        // so remove/add to put it in the correct place.
        message.fields().remove(crocofix::FIX_5_0SP2::field::CheckSum::Tag);
        message.fields().set(crocofix::FIX_5_0SP2::field::CheckSum::Tag, "", set_operation::replace_first_or_append); // encode will recalc this
        std::vector<char> buffer(1024);
        size_t length = 0;
        while ((length = message.encode(std::span(buffer.data(), buffer.size()), options)) == 0) {
            buffer.resize(buffer.size() * 1.5);    
        }
        return std::string(buffer.data(), length);
    }


};

}

#endif