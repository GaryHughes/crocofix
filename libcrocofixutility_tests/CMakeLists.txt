add_executable(libcrocofixutility_tests
    read_file_tests.cpp
    number_of_digits_tests.cpp
    blocking_queue_tests.cpp
    report_tests.cpp
)

target_link_libraries(libcrocofixutility_tests
    crocofixutility
    ${BOOST_LIBS}
    Catch2::Catch2WithMain
)

catch_discover_tests(libcrocofixutility_tests WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

# clang-tidy is crashing on blocking_queue_tests so turn off for this module.
set_target_properties(
    libcrocofixutility_tests
    PROPERTIES
         CXX_CLANG_TIDY ""
)