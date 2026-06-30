add_executable(libcrocofixlua_tests
    lua_fixture.hpp
    message_tests.cpp
)

find_package(Lua 5.4 REQUIRED)
target_include_directories(libcrocofixlua_tests PRIVATE ${LUA_INCLUDE_DIR})

if(APPLE)
    # sol2 prefers <lua/lua.h> over <lua.h>; add the keg's parent dir so that
    # form resolves to the Lua 5.4 keg too, instead of falling through to
    # /opt/homebrew/include/lua (the unversioned, Lua 5.5 formula).
    target_include_directories(libcrocofixlua_tests PRIVATE "/opt/homebrew/opt/lua@5.4/include")
endif()

target_link_libraries(libcrocofixlua_tests
    crocofixlua
    ${BOOST_LIBS}
    ${LUA_LIBRARIES}
    Catch2::Catch2WithMain
)

catch_discover_tests(libcrocofixlua_tests)
