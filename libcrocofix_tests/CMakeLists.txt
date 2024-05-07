add_executable(libcrocofix_tests
    field_tests.cpp
    message_tests.cpp
    session_state_tests.cpp
    session_tests.cpp
    session_fixture.hpp session_fixture.cpp
    test_reader.hpp
    test_writer.hpp
    field_collection_tests.cpp
    order_tests.cpp
    order_book_tests.cpp
)

target_link_libraries(libcrocofix_tests
    crocofix
    crocofixutility
    ${BOOST_LIBS}
    Catch2::Catch2WithMain
)

catch_discover_tests(libcrocofix_tests)
