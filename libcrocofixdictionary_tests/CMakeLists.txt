add_executable(libcrocofixdictionary_tests
    field_tests.cpp
    message_tests.cpp
    orchestration_tests.cpp
)

target_link_libraries(libcrocofixdictionary_tests
    crocofixdictionary
    ${BOOST_LIBS}
    Catch2::Catch2WithMain
)

catch_discover_tests(libcrocofixdictionary_tests)