#ifndef crocofix_libcrocofixpython_writer_hpp
#define crocofix_libcrocofixpython_writer_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_writer(py::module_& module);

#endif