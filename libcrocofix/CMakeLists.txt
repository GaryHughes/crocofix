
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

add_library(crocofix)

target_sources(crocofix
    PUBLIC
        FILE_SET CXX_MODULES FILES
            field.cppm
            field_collection.cppm
            message.cppm
            session.cppm
            reader.cppm
            writer.cppm
            socket_reader.cppm
            socket_writer.cppm 
            behaviour.cppm
            session_state.cppm
            session_options.cppm
            scheduler.cppm
            boost_asio_scheduler.cppm
            order.cppm
            order_book.cppm
            order_report.cppm
            crocofix.cppm
    PRIVATE
        field.cpp
        field_collection.cpp
        message.cpp
        session.cpp
        socket_reader.cpp
        socket_writer.cpp
        session_state.cpp
        boost_asio_scheduler.cpp
        order.cpp
        order_book.cpp
        order_report.cpp
        ${DTRACE_SOURCES}
        ${DTRACE_OBJS}
)

target_link_libraries(crocofix
    crocofixutility
    crocofixdictionary
    ${BOOST_LIBS}
)
