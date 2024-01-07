#ifndef crocofix_libcrocofixpython_reader_hpp
#define crocofix_libcrocofixpython_reader_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_reader(py::module_& module);

#endif