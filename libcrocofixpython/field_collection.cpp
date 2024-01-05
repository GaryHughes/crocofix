#include "field_collection.hpp"
#include <libcrocofix/field_collection.hpp>

void init_field_collection(py::module_& module)
{
    py::class_<crocofix::field_collection>(module, "FieldCollection")
        .def("clear", &crocofix::field_collection::clear)
        .def("__len__", [](const crocofix::field_collection& collection) { return collection.size(); })
        .def("__iter__", [](crocofix::field_collection& collection) {
            return py::make_iterator(collection.begin(), collection.end());
        }, py::keep_alive<0, 1>())
    ;
}
