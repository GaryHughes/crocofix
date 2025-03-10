#ifndef crocofix_libcrocofixpython_field_hpp
#define crocofix_libcrocofixpython_field_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_field(py::module_& module);

#endif