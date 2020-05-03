add_library(crocofix
    field.hpp field.cpp
    field_collection.hpp field_collection.cpp
    message.hpp message.cpp
)

target_link_libraries(crocofix
    crocofixdictionary
    ${BOOST_LIBS}
)
