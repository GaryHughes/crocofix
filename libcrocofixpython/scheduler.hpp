#ifndef crocofix_libcrocofixpython_scheduler_hpp
#define crocofix_libcrocofixpython_scheduler_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_scheduler(py::module_& module);

#endif