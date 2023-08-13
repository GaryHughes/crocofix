add_library(crocofixutility
    read_file.hpp read_file.cpp
    number_of_digits.hpp
    blocking_queue.hpp
    report.hpp report.cpp
)

target_link_libraries(crocofixutility
    ${BOOST_LIBS}
)
