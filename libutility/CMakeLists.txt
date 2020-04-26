add_library(utility
    read_file.hpp read_file.cpp
)

target_link_libraries(utility
    ${BOOST_LIBS}
)
