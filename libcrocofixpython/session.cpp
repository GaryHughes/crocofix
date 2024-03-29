#include "session.hpp"
#include <pybind11/functional.h>
#include <libcrocofix/session.hpp>
#include <libcrocofixdictionary/fix50SP2_orchestration.hpp>

void init_session(py::module_& module)
{
    py::enum_<crocofix::behaviour>(module, "Behaviour")
        .value("INITIATOR", crocofix::behaviour::initiator)
        .value("ACCEPTOR", crocofix::behaviour::acceptor)
    ;

    py::class_<crocofix::session>(module, "Session")

        .def(py::init([](crocofix::reader& reader, crocofix::writer& writer, crocofix::scheduler& scheduler) {
            return std::make_shared<crocofix::session>(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());
        }))
        .def("open", &crocofix::session::open)
        .def("close", &crocofix::session::close)
        .def_property("test_request_delay", &crocofix::session::test_request_delay, &crocofix::session::test_request_delay)
        .def_property("heartbeat_interval", &crocofix::session::heartbeat_interval, &crocofix::session::heartbeat_interval)
        .def_property("LogonBehaviour", &crocofix::session::logon_behaviour, &crocofix::session::logon_behaviour)
        .def_property("BeginString", &crocofix::session::begin_string, &crocofix::session::begin_string)
        .def_property("SenderCompID", &crocofix::session::sender_comp_id, &crocofix::session::sender_comp_id)
        .def_property("TargetCompID", &crocofix::session::target_comp_id, &crocofix::session::target_comp_id)
    ;

}
