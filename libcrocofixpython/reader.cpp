#include "reader.hpp"
#include <libcrocofix/reader.hpp>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>

namespace py = pybind11;

class Reader : public crocofix::reader
{
public:

    void read_async(crocofix::reader::message_callback callback) override 
    {
        PYBIND11_OVERRIDE_PURE(void, crocofix::reader, read_async, callback );
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
        .def("read_async", &crocofix::reader::read_async)
        .def("close", &crocofix::reader::close)
    ;
}