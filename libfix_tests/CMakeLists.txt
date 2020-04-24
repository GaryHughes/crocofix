add_executable(libfix_tests
    main.cpp
)

target_link_libraries(libfix_tests
    fix
    ${BOOST_LIBS}
)

add_test(NAME libfix_tests 
    COMMAND libfix_tests
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
