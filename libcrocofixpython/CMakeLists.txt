find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED) # brew install pybind11

pybind11_add_module(pycrocofix module.cpp)

set_target_properties(pycrocofix PROPERTIES OUTPUT_NAME crocofix)
set_target_properties(pycrocofix PROPERTIES SUFFIX ".so")

target_link_libraries(pycrocofix PUBLIC
    crocofix
    crocofixutility
    ${BOOST_LIBS}
)
