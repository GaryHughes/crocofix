add_executable(fixcat 
    main.cpp
    options.hpp options.cpp
)

target_link_libraries(fixcat 
    crocofix
    crocofixdictionary
    crocofixutility
    ${BOOST_LIBS}
)