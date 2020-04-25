add_library(crocofix
    field.hpp field.cpp
)

target_link_libraries(crocofix
    ${BOOST_LIBS}
)
