add_library(crocofix
    field.hpp field.cpp
    field_collection.hpp field_collection.cpp
    message.hpp message.cpp
    session.hpp session.cpp
    reader.hpp
    writer.hpp
    socket_reader.hpp socket_reader.cpp
    socket_writer.hpp socket_writer.cpp
)

target_link_libraries(crocofix
    crocofixdictionary
    ${BOOST_LIBS}
)
