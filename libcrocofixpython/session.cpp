#include "session.hpp"
#include <pybind11/functional.h>
#include <libcrocofix/session.hpp>
#include <libcrocofixdictionary/fix50SP2_orchestration.hpp>

class session_proxy : public crocofix::session
{
public:

    session_proxy(crocofix::reader& reader, 
                  crocofix::writer& writer,
                  crocofix::scheduler& scheduler,
                  const crocofix::dictionary::orchestration& orchestration)
    :   crocofix::session(reader, writer, scheduler, orchestration)
    {
        // TODO - can we do this on demand?
        m_information_connection = information.connect([&](const auto& message) { information_callback(message); });
        m_warning_connection = warning.connect([&](const auto& message) { warning_callback(message); });
        m_error_connection = error.connect([&](const auto& message) { error_callback(message); });
        m_state_changed_connection = state_changed.connect([&](crocofix::session_state from, crocofix::session_state to) { state_changed_callback(from, to); }); // NOLINT(readability-identifier-length)
        m_message_sent_connection = message_sent.connect([&](const crocofix::message& message) { message_sent_callback(message); } );
        m_message_received_connection = message_received.connect([&](const crocofix::message& message) { message_received_callback(message); });
    }

    using log_callback = std::function<void(const std::string&)>;
    using state_change_callback = std::function<void(crocofix::session_state, crocofix::session_state)>;
    using message_callback = std::function<void(const crocofix::message&)>;

    log_callback information_callback;
    log_callback warning_callback;
    log_callback error_callback;
   
    state_change_callback state_changed_callback;

    message_callback message_sent_callback;
    message_callback message_received_callback;

private:

    boost::signals2::connection m_information_connection;
    boost::signals2::connection m_warning_connection;
    boost::signals2::connection m_error_connection;

    boost::signals2::connection m_state_changed_connection;

    boost::signals2::connection m_message_sent_connection;
    boost::signals2::connection m_message_received_connection;

};

void init_session(py::module_& module)
{
    py::enum_<crocofix::behaviour>(module, "Behaviour")
        .value("INITIATOR", crocofix::behaviour::initiator)
        .value("ACCEPTOR", crocofix::behaviour::acceptor)
    ;

    py::enum_<crocofix::session_state>(module, "SessionState")
        .value("DISCONNECTED", crocofix::session_state::disconnected)
        .value("CONNECTED", crocofix::session_state::connected)
        .value("LOGGING_ON", crocofix::session_state::logging_on)
        .value("RESENDING", crocofix::session_state::resending)
        .value("LOGGED_ON", crocofix::session_state::logged_on)
        .value("RESETTING", crocofix::session_state::resetting)
    ;

    py::class_<session_proxy>(module, "Session")

        .def(py::init([](crocofix::reader& reader, crocofix::writer& writer, crocofix::scheduler& scheduler) {
            return std::make_shared<session_proxy>(reader, writer, scheduler, crocofix::FIX_5_0SP2::orchestration());
        }))

        .def("open", &crocofix::session::open)
        .def("close", &crocofix::session::close)

        .def_property("test_request_delay",
                      static_cast<uint32_t(crocofix::session::*)()const>(&crocofix::session::test_request_delay),
                      static_cast<void(crocofix::session::*)(uint32_t)>(&crocofix::session::test_request_delay))
        
        .def_property("heartbeat_interval",
                      static_cast<uint32_t(crocofix::session::*)()const>(&crocofix::session::heartbeat_interval),
                      static_cast<void(crocofix::session::*)(uint32_t)>(&crocofix::session::heartbeat_interval))

        .def_property("LogonBehaviour",
                      static_cast<crocofix::behaviour(crocofix::session::*)()const>(&crocofix::session::logon_behaviour),
                      static_cast<void(crocofix::session::*)(crocofix::behaviour)>(&crocofix::session::logon_behaviour))

        .def_property("BeginString",
                      static_cast<const std::string&(crocofix::session::*)()const>(&crocofix::session::begin_string), 
                      static_cast<void(crocofix::session::*)(const std::string&)>(&crocofix::session::begin_string))

        .def_property("SenderCompID",
                      static_cast<const std::string&(crocofix::session::*)()const>(&crocofix::session::sender_comp_id), 
                      static_cast<void(crocofix::session::*)(const std::string&)>(&crocofix::session::sender_comp_id))

        .def_property("TargetCompID",
                      static_cast<const std::string&(crocofix::session::*)()const>(&crocofix::session::target_comp_id), 
                      static_cast<void(crocofix::session::*)(const std::string&)>(&crocofix::session::target_comp_id))

        .def_readwrite("information", &session_proxy::information_callback)
        .def_readwrite("warning", &session_proxy::warning_callback)
        .def_readwrite("error", &session_proxy::error_callback)
        .def_readwrite("state_changed", &session_proxy::state_changed_callback)
        .def_readwrite("message_sent", &session_proxy::message_sent_callback)
        .def_readwrite("message_received", &session_proxy::message_received_callback)
    ;

}
