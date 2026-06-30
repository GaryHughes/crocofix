add_library(crocofixlua)

target_sources(crocofixlua
    PUBLIC
        FILE_SET CXX_MODULES FILES
            types.cppm
            crocofixlua.cppm
    PRIVATE
        types.cpp
        ${DTRACE_OBJS}
)

if(APPLE)
    # Homebrew's unversioned "lua" formula is 5.5, which sol2 does not support.
    # The real 5.4 formula is keg-only and not discovered by CMake's FindLua, so
    # point it there explicitly. This dir must contain lua.h directly, or
    # find_package(Lua) will treat the forced cache value as stale and re-search.
    set(LUA_INCLUDE_DIR "/opt/homebrew/opt/lua@5.4/include/lua5.4" CACHE PATH "" FORCE)
    set(LUA_LIBRARY "/opt/homebrew/opt/lua@5.4/lib/liblua.5.4.dylib" CACHE FILEPATH "" FORCE)
endif()

find_package(Lua 5.4 REQUIRED)
find_package(sol2 CONFIG REQUIRED)

target_include_directories(crocofixlua PRIVATE ${LUA_INCLUDE_DIR})

if(APPLE)
    # sol2 prefers <lua/lua.h> over <lua.h>; add the keg's parent dir so that
    # form resolves here too, instead of falling through to
    # /opt/homebrew/include/lua (the unversioned, Lua 5.5 formula).
    target_include_directories(crocofixlua PRIVATE "/opt/homebrew/opt/lua@5.4/include")
endif()

target_link_libraries(crocofixlua
    crocofix
    sol2::sol2
)