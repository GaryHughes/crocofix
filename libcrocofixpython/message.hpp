#ifndef crocofix_libcrocofixpython_message_hpp
#define crocofix_libcrocofixpython_message_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_message(py::module_& module);

#endif