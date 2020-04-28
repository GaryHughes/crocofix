add_executable(fixsed
    main.cpp
    options.hpp options.cpp
    pipeline.hpp pipeline.cpp
)

target_link_libraries(fixsed
    crocofix
    crocofixdictionary
    utility 
    ${BOOST_LIBS}
)