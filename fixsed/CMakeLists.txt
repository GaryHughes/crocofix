set(DTRACE_OBJS, "")

if(DTRACE)
    if(APPLE)
        execute_process(COMMAND ${DTRACE} -h -s ${CMAKE_CURRENT_SOURCE_DIR}/probes.d -o ${CMAKE_CURRENT_SOURCE_DIR}/probes.h)
    else()
        execute_process(COMMAND ${DTRACE} -G -s ${CMAKE_CURRENT_SOURCE_DIR}/probes.d -o ${CMAKE_CURRENT_BINARY_DIR}/probes.o)
        execute_process(COMMAND ${DTRACE} -h -s ${CMAKE_CURRENT_SOURCE_DIR}/probes.d -o ${CMAKE_CURRENT_SOURCE_DIR}/probes.h)
        set(DTRACE_OBJS "${CMAKE_CURRENT_BINARY_DIR}/probes.o")
    endif()
endif()

set(DTRACE_SOURCES probes.h)

add_executable(fixsed
    main.cpp
    options.hpp options.cpp
    pipeline.hpp pipeline.cpp
    logging.hpp logging.cpp
    ${DTRACE_SOURCES}
    ${DTRACE_OBJS}
)

find_package(Lua 5.4 REQUIRED)
target_include_directories(fixsed PRIVATE ${LUA_INCLUDE_DIR})

if(APPLE)
    # sol2 prefers <lua/lua.h> over <lua.h>; add the keg's parent dir so that
    # form resolves to the Lua 5.4 keg too, instead of falling through to
    # /opt/homebrew/include/lua (the unversioned, Lua 5.5 formula).
    target_include_directories(fixsed PRIVATE "/opt/homebrew/opt/lua@5.4/include")
endif()

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(fixsed PRIVATE
    crocofix
    crocofixdictionary
    crocofixutility
    crocofixlua 
    ${BOOST_LIBS}
    ${LUA_LIBRARIES}
    spdlog::spdlog
)
