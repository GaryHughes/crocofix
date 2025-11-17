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

find_package(Lua 5.4 REQUIRED)
find_package(sol2 CONFIG REQUIRED)

target_include_directories(crocofixlua PRIVATE ${LUA_INCLUDE_DIR})

target_link_libraries(crocofixlua
    crocofix
    sol2::sol2
)