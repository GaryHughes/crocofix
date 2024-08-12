find_package(Python REQUIRED COMPONENTS Interpreter Development)

message("PYTHON ${Python_EXECUTABLE}")
message("PYTHON ${Python_STDLIB}")

add_subdirectory("pybind11")
include_directories("pybind11/include")

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

configure_file(crocofix/asyncio/__init__.py crocofix/asyncio/__init__.py COPYONLY)
configure_file(crocofix/asyncio/scheduler.py crocofix/asyncio/scheduler.py COPYONLY)
configure_file(crocofix/asyncio/socket_reader.py crocofix/asyncio/socket_reader.py COPYONLY)
configure_file(crocofix/asyncio/socket_writer.py crocofix/asyncio/socket_writer.py COPYONLY)
