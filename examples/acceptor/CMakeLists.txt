add_executable(acceptor 
    main.cpp
)

target_link_libraries(acceptor 
    crocofix
    ${BOOST_LIBS}
)