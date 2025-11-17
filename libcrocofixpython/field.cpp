#include "field.hpp"

import crocofix;

void init_field(py::module_& module)
{
    py::enum_<crocofix::timestamp_format>(module, "_TimestampFormat")
        .value("SECONDS", crocofix::timestamp_format::seconds)
        .value("MILLISECONDS", crocofix::timestamp_format::milliseconds)
    ;

    module.def("_timestamp_string", &crocofix::timestamp_string);

    py::class_<crocofix::field>(module, "_Field")
        .def(py::init<int, std::string>())
        .def(py::init<int, bool>())
        .def(py::init<int, double>())
        .def(py::init<int, int64_t>())
        .def_property_readonly("tag", &crocofix::field::tag)
        .def_property_readonly("value", &crocofix::field::value)
    ;

}