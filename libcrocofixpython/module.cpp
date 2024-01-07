#include <pybind11/pybind11.h>
#include "field.hpp"
#include "field_collection.hpp"
#include "message.hpp"
#include "session.hpp"
#include "reader.hpp"
#include "writer.hpp"

PYBIND11_MODULE(crocofix, module) { 

    module.doc() = "This is a Python binding of CrocoFIX";

    init_field(module);
    init_field_collection(module);
    init_message(module);
    init_reader(module);
    init_writer(module);
    init_session(module);
}
