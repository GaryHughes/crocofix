add_executable(fixsed
    main.cpp
    options.hpp options.cpp
    pipeline.hpp pipeline.cpp
    logging.hpp logging.cpp
)

find_package(Lua 5.4 REQUIRED)

target_include_directories(fixsed PRIVATE ${LUA_INCLUDE_DIR})

target_include_directories(fixsed PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty)

target_link_libraries(fixsed
    crocofix
    crocofixdictionary
    crocofixutility 
    ${BOOST_LIBS}
    ${LUA_LIBRARIES}
)