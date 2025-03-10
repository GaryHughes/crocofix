#include "reader.hpp"
#include <libcrocofix/reader.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

namespace py = pybind11;

void init_reader(py::module_& module)
{
    py::class_<Reader>(module, "_Reader")
        .def(py::init<>())
        .def("open", &crocofix::reader::open)
        .def("close", &crocofix::reader::close)
        .def("dispatch_message_read", &crocofix::reader::dispatch_message_read)

        .def_readwrite("opened", &Reader::opened_callback)
        .def_readwrite("closed", &Reader::closed_callback)
        .def_readwrite("message_read", &Reader::message_read_callback)
    ;
}