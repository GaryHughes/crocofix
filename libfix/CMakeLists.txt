add_library(fix
    field.hpp field.cpp
)

target_link_libraries(fix
    ${BOOST_LIBS}
)
