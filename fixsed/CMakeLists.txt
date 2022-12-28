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

add_executable(fixsed
    main.cpp
    options.hpp options.cpp
    pipeline.hpp pipeline.cpp
    logging.hpp logging.cpp
    telemetry.hpp telemetry.cpp
    ${DTRACE_SOURCES}
    ${DTRACE_OBJS}
)

find_package(Lua 5.4 REQUIRED)
target_include_directories(fixsed PRIVATE ${LUA_INCLUDE_DIR})
target_include_directories(fixsed PRIVATE ${CMAKE_SOURCE_DIR}/thirdparty)

target_link_libraries(fixsed
    crocofix
    crocofixdictionary
    crocofixutility
    crocofixlua 
    ${BOOST_LIBS}
    ${LUA_LIBRARIES}
    opentelemetry_exporter_otlp_http_metric
    opentelemetry_exporter_otlp_http_client
    opentelemetry_exporter_otlp_http
    opentelemetry_otlp_recordable
    opentelemetry_metrics
    opentelemetry_proto
    opentelemetry_http_client_curl
    opentelemetry_resources
    opentelemetry_common
    curl
    protobuf
)
