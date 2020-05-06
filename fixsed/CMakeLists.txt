add_executable(fixsed
    main.cpp
    options.hpp options.cpp
    pipeline.hpp pipeline.cpp
    logging.hpp logging.cpp
)

target_link_libraries(fixsed
    crocofix
    crocofixdictionary
    crocofixutility 
    ${BOOST_LIBS}
)