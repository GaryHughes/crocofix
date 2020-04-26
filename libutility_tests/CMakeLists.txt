add_executable(libutility_tests
    main.cpp
    read_file_tests.cpp
)

target_link_libraries(libutility_tests
    utility
    ${BOOST_LIBS}
)

add_test(NAME libutility_tests 
    COMMAND libutility_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
