
set(DTRACE_OBJS, "")

if(DTRACE)
    if(APPLE)
        execute_process(COMMAND ${DTRACE} -h -s ${CMAKE_CURRENT_SOURCE_DIR}/probes.d -o ${CMAKE_CURRENT_SOURCE_DIR}/probes.h)
    else()
        execute_process(COMMAND ${DTRACE} -G -s ${CMAKE_CURRENT_SOURCE_DIR}/probes.d -o ${CMAKE_CURRENT_BINARY_DIR}/probes.o)
        execute_process(COMMAND ${DTRACE} -h -s ${CMAKE_CURRENT_SOURCE_DIR}/probes.d -o ${CMAKE_CURRENT_SOURCE_DIR}/probes.h)
        set(DTRACE_OBJS "${CMAKE_CURRENT_BINARY_DIR}/probes.o")
    endif()
endif()

set(DTRACE_SOURCES probes.h)

add_library(crocofix
    field.hpp field.cpp
    field_collection.hpp field_collection.cpp
    message.hpp message.cpp
    session.hpp session.cpp
    reader.hpp
    writer.hpp
    socket_reader.hpp socket_reader.cpp
    socket_writer.hpp socket_writer.cpp
    behaviour.hpp
    session_state.hpp session_state.cpp
    session_options.hpp
    scheduler.hpp
    boost_asio_scheduler.hpp boost_asio_scheduler.cpp
    order.hpp order.cpp
    order_book.hpp order_book.cpp
    ${DTRACE_SOURCES}
    ${DTRACE_OBJS}
)

target_link_libraries(crocofix
    crocofixdictionary
    ${BOOST_LIBS}
)
