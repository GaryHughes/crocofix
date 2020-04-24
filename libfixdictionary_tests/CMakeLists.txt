add_executable(libfixdictionary_tests
    main.cpp
)

target_link_libraries(libfixdictionary_tests
    fixdictionary
    ${BOOST_LIBS}
)

add_test(NAME libfixdictionary_tests 
    COMMAND libfixdictionary_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
