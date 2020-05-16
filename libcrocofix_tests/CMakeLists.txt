add_executable(libcrocofix_tests
    main.cpp
    message_tests.cpp
    session_state_tests.cpp
    session_tests.cpp
    session_fixture.hpp session_fixture.cpp
    test_reader.hpp
    test_writer.hpp
)

target_link_libraries(libcrocofix_tests
    crocofix
    ${BOOST_LIBS}
)

add_test(NAME libcrocofix_tests 
    COMMAND libcrocofix_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
