find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
find_package(pybind11 CONFIG REQUIRED) # brew install pybind11

message(${Python_EXECUTABLE})

pybind11_add_module(pycrocofix module.cpp)

target_link_libraries(pycrocofix PUBLIC
    crocofix
    crocofixutility
    ${BOOST_LIBS}
)
