add_executable(libcrocofixdictionary_tests
    main.cpp
    field_tests.cpp
    message_tests.cpp
    orchestration_tests.cpp
)

target_link_libraries(libcrocofixdictionary_tests
    crocofixdictionary
    ${BOOST_LIBS}
)

catch_discover_tests(libcrocofixdictionary_tests)