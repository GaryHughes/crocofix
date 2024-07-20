#include "reader.hpp"
#include <libcrocofix/reader.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

namespace py = pybind11;

class Reader : public crocofix::reader
{
public:

    void open() override
    {
        // TODO - can we do this to avoid crashes for not implemented methods?
        // pybind11::gil_scoped_acquire gil;  // Acquire the GIL while in this scope.
        // // Try to look up the overridden method on the Python side.
        // pybind11::function override = pybind11::get_override(this, "myMethod");
        // if (override) {  // method is found
        PYBIND11_OVERRIDE_PURE(void, crocofix::reader, open,);
    }

    void close() override 
    {
        PYBIND11_OVERRIDE_PURE(void, crocofix::reader, close,);
    }

};

void init_reader(py::module_& module)
{
    py::class_<crocofix::reader, Reader>(module, "Reader")
        .def(py::init<>())
        .def("open", &crocofix::reader::open)
        .def("close", &crocofix::reader::close)
        .def("dispatch_message_read", &crocofix::reader::dispatch_message_read)
    ;
}