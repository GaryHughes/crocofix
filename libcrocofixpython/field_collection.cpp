#include "field_collection.hpp"
#include <libcrocofix/field_collection.hpp>

void init_field_collection(py::module_& module)
{
    py::enum_<crocofix::set_operation>(module, "_SetOperation")
        .value("REPLACE_FIRST", crocofix::set_operation::replace_first)
        .value("REPLACE_FIRST_OR_APPEND", crocofix::set_operation::replace_first_or_append)
        .value("APPEND", crocofix::set_operation::append)
    ;

    py::enum_<crocofix::remove_operation>(module, "_RemoveOperation")
        .value("REMOVE_FIRST", crocofix::remove_operation::remove_first)
        .value("REMOVE_ALL", crocofix::remove_operation::remove_all)
    ;

    py::class_<crocofix::field_collection>(module, "_FieldCollection")
        .def(py::init())
        .def("set", static_cast<bool(crocofix::field_collection::*)(int, std::string, crocofix::set_operation)>(&crocofix::field_collection::set))
        .def("set", static_cast<bool(crocofix::field_collection::*)(int, bool, crocofix::set_operation)>(&crocofix::field_collection::set))
        .def("set", static_cast<bool(crocofix::field_collection::*)(int, double, crocofix::set_operation)>(&crocofix::field_collection::set))
        .def("set", static_cast<bool(crocofix::field_collection::*)(int, int64_t, crocofix::set_operation)>(&crocofix::field_collection::set))
        .def("set", static_cast<bool(crocofix::field_collection::*)(const crocofix::field&, crocofix::set_operation)>(&crocofix::field_collection::set))
        // TODO - set via dictionary::field_value
        .def("get", &crocofix::field_collection::get)
        // try_get isn't really idiomatic so leave out for now
        .def("remove", &crocofix::field_collection::remove)
        .def("clear", &crocofix::field_collection::clear)
        .def("__len__", [](const crocofix::field_collection& collection) { return collection.size(); })
        .def("__iter__", [](crocofix::field_collection& collection) {
            return py::make_iterator(collection.begin(), collection.end());
        }, py::keep_alive<0, 1>())
    ;
}
   