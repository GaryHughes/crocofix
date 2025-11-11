add_executable(libcrocofixlua_tests
    lua_fixture.hpp
    message_tests.cpp
)

find_package(Lua 5.4 REQUIRED)
target_include_directories(libcrocofixlua_tests PRIVATE ${LUA_INCLUDE_DIR})

target_link_libraries(libcrocofixlua_tests
    crocofixlua
    ${BOOST_LIBS}
    ${LUA_LIBRARIES}
    Catch2::Catch2WithMain
)

catch_discover_tests(libcrocofixlua_tests)
