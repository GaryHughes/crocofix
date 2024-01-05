#include "message.hpp"
#include <libcrocofix/message.hpp>
#include <gsl/gsl_util>
#include <sstream>

void init_message(py::module_& module)
{
    py::class_<crocofix::message::decode_result>(module, "DecodeResult")
        .def_readonly("consumed", &crocofix::message::decode_result::consumed)
        .def_readonly("complete", &crocofix::message::decode_result::complete)
    ;

    py::class_<crocofix::message>(module, "Message")
        .def(py::init<>())
        .def_property_readonly("BeginString", &crocofix::message::BeginString)
        .def_property_readonly("SenderCompID", &crocofix::message::SenderCompID)
        .def_property_readonly("TargetCompID", &crocofix::message::TargetCompID)
        .def_property_readonly("MsgType", &crocofix::message::MsgType)
        .def_property_readonly("MsgSeqNum", &crocofix::message::MsgSeqNum)
        .def_property_readonly("PossDupFlag", &crocofix::message::PossDupFlag)
        .def_property_readonly("GapFillFlag", &crocofix::message::GapFillFlag)
        .def_property_readonly("ResetSeqNumFlag", &crocofix::message::ResetSeqNumFlag)
        .def_property_readonly("is_admin", &crocofix::message::is_admin)
        .def_property_readonly("fields", static_cast<crocofix::field_collection&(crocofix::message::*)()>(&crocofix::message::fields)) // cast to select the non const overload
        .def("reset", &crocofix::message::reset)
        .def("calculate_body_length", &crocofix::message::calculate_body_length)
        .def("calculate_checksum", static_cast<uint32_t(crocofix::message::*)() const>(&crocofix::message::calculate_checksum)) // cast to select the member over the static
        .def("decode", &crocofix::message::decode)
        .def("__repr__",
            [](crocofix::message& message) {
                std::vector<char> buffer(1024);
                for (;;) {
                    auto result = message.encode(std::span(buffer.data(), buffer.size()));
                    if (result == 0) {
                        buffer.resize(gsl::narrow<size_t>(buffer.size() * 1.5)); // NOLINT(bugprone-narrowing-conversions,cppcoreguidelines-narrowing-conversions)
                        continue;
                    }
                    return std::string(buffer.data(), result);
                }
            }
        )
        .def("__str__",
            [](const crocofix::message& message) {
                std::ostringstream buffer;
                message.pretty_print(buffer);
                return buffer.str();
            }
        )
    ;
}