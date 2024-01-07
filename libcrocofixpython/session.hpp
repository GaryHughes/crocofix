#ifndef crocofix_libcrocofixpython_session_hpp
#define crocofix_libcrocofixpython_session_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_session(py::module_& module);

#endif