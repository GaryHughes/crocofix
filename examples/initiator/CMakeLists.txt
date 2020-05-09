add_executable(initiator
    main.cpp
)

target_link_libraries(initiator
    crocofix
    ${BOOST_LIBS}
)