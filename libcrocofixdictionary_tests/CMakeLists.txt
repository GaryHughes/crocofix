add_executable(libcrocofixdictionary_tests
    main.cpp
)

target_link_libraries(libcrocofixdictionary_tests
    crocofixdictionary
    ${BOOST_LIBS}
)

add_test(NAME libcrocofixdictionary_tests 
    COMMAND libcrocofixdictionary_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
