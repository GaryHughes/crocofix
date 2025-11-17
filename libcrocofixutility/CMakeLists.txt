add_library(crocofixutility)

target_sources(crocofixutility
    PUBLIC
        FILE_SET CXX_MODULES FILES 
            blocking_queue.cppm
            number_of_digits.cppm
            read_file.cppm
            report.cppm
            crocofixutility.cppm 
    PRIVATE 
        read_file.cpp
        report.cpp
)

target_link_libraries(crocofixutility
    ${BOOST_LIBS}
)

