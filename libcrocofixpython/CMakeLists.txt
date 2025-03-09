find_package(Python REQUIRED COMPONENTS Interpreter Development)

message("PYTHON ${Python_EXECUTABLE}")
message("PYTHON ${Python_STDLIB}")

include_directories(${CMAKE_SOURCE_DIR}/thirdparty/pybind11/include)

add_definitions(-DPYBIND11_USE_SMART_HOLDER_AS_DEFAULT)

pybind11_add_module(pycrocofix 
    module.cpp
    field.cpp field.hpp
    field_collection.cpp field_collection.hpp
    message.cpp message.hpp
    session.cpp session.hpp
    reader.cpp reader.hpp
    writer.cpp writer.hpp
    scheduler.cpp scheduler.hpp
)

set_target_properties(pycrocofix PROPERTIES LIBRARY_OUTPUT_DIRECTORY crocofix)
set_target_properties(pycrocofix PROPERTIES OUTPUT_NAME _crocofix)
set_target_properties(pycrocofix PROPERTIES SUFFIX ".so")

target_link_libraries(pycrocofix PUBLIC
    crocofix
    crocofixutility
    ${BOOST_LIBS}
)

# TODO - find a better way to this without listing the files.
configure_file(crocofix/__init__.py crocofix/__init__.py COPYONLY)
configure_file(crocofix/field.py crocofix/field.py COPYONLY)
configure_file(crocofix/message.py crocofix/message.py COPYONLY)
configure_file(crocofix/session.py crocofix/session.py COPYONLY)

configure_file(crocofix/sockets/__init__.py crocofix/sockets/__init__.py COPYONLY)
configure_file(crocofix/sockets/scheduler.py crocofix/sockets/scheduler.py COPYONLY)
configure_file(crocofix/sockets/socket_reader.py crocofix/sockets/socket_reader.py COPYONLY)
configure_file(crocofix/sockets/socket_writer.py crocofix/sockets/socket_writer.py COPYONLY)

configure_file(crocofix/streams/__init__.py crocofix/streams/__init__.py COPYONLY)
configure_file(crocofix/streams/scheduler.py crocofix/streams/scheduler.py COPYONLY)
configure_file(crocofix/streams/stream_reader.py crocofix/streams/stream_reader.py COPYONLY)
configure_file(crocofix/streams/stream_writer.py crocofix/streams/stream_writer.py COPYONLY)
