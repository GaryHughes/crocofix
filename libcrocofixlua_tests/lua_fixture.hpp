#ifndef libcrocofixlua_tests_lua_fixture_hpp
#define libcrocofixlua_tests_lua_fixture_hpp

#include <libcrocofixlua/message_type.hpp>

namespace crocofix
{

struct lua_fixture
{
    lua_fixture()
    {
        initialise_message_type(lua);
    }

    sol::state lua;

};

}

#endif