add_executable(libcrocofix_tests
    main.cpp
    message_tests.cpp
)

target_link_libraries(libcrocofix_tests
    crocofix
    ${BOOST_LIBS}
)

add_test(NAME libcrocofix_tests 
    COMMAND libcrocofix_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
