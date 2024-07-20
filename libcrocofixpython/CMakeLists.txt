find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

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

set_target_properties(pycrocofix PROPERTIES OUTPUT_NAME _crocofix)
set_target_properties(pycrocofix PROPERTIES SUFFIX ".so")

target_link_libraries(pycrocofix PUBLIC
    crocofix
    crocofixutility
    ${BOOST_LIBS}
)


add_custom_command(TARGET pycrocofix PRE_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/libcrocofixpython/asyncio/ $<TARGET_FILE_DIR:pycrocofix>/asyncio)

configure_file(crocofix.py crocofix.py COPYONLY)

add_custom_target(python_package_build
                  COMMAND "${PYTHON_EXECUTABLE}" -m pip wheel .
                  WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
                  COMMENT "Building python wheel package")

add_dependencies(python_package_build pycrocofix)