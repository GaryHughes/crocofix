#include "writer.hpp"
#include <libcrocofix/writer.hpp>

class Writer : public crocofix::writer
{
public:

    void write(crocofix::message& message, int options = crocofix::encode_options::standard) override
    {
        // PYBIND11_OVERRIDE_PURE passes by value so send the address of message.
        PYBIND11_OVERRIDE_PURE(void, crocofix::writer, write, &message, options);
    }

    void close() override 
    {
        PYBIND11_OVERRIDE_PURE(void, crocofix::writer, close,);
    }

};

void init_writer(py::module_& module)
{
    py::class_<crocofix::writer, Writer>(module, "Writer")
        .def(py::init<>())
        .def("write", &crocofix::writer::write)
        .def("close", &crocofix::writer::close)
    ;

}