#ifndef crocofix_libcrocofixpython_field_collection_hpp
#define crocofix_libcrocofixpython_field_collection_hpp

#include <pybind11/pybind11.h>

namespace py = pybind11;

void init_field_collection(py::module_& module);

#endif