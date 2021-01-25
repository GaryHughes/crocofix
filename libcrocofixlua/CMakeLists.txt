add_library(crocofixlua
    message_type.cpp
    ${DTRACE_OBJS}
)

find_package(Lua 5.4 REQUIRED)
target_include_directories(crocofixlua PRIVATE ${LUA_INCLUDE_DIR})
target_include_directories(crocofixlua PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty)

target_link_libraries(crocofixlua
    crocofix
)