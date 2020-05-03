add_executable(libcrocofixutility_tests
    main.cpp
    read_file_tests.cpp
    number_of_digits_tests.cpp
)

target_link_libraries(libcrocofixutility_tests
    crocofixutility
    ${BOOST_LIBS}
)

add_test(NAME libcrocofixutility_tests 
    COMMAND libcrocofixutility_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
