add_library(crocofixutility
    read_file.hpp read_file.cpp
)

target_link_libraries(crocofixutility
    ${BOOST_LIBS}
)
