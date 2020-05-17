#include "session.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <functional>
#include <stdexcept>


#include <iostream>

namespace crocofix
{

session::session(reader& reader, writer& writer)
:   m_reader(reader),
    m_writer(writer)
{
     m_reader.read_async([=](crocofix::message& message) { on_message_read(message); });
}    

void session::open()
{
    logon();
}

void session::close()
{
}

void session::on_message_read(crocofix::message& message)
{
    if (!message.is_admin()) {
        return;
    }

    if (message.MsgType() == FIX_5_0SP2::message::Logon::MsgType) {
        if (!process_logon(message)) {
            return;
        }
    }

    if (message.MsgType() == FIX_5_0SP2::message::Heartbeat::MsgType) {
        process_heartbeat(message);
    }
    else if (message.MsgType() == FIX_5_0SP2::message::TestRequest::MsgType) {
        process_test_request(message);
    }
}

void session::logon()
{
    if (state() != session_state::connected) {
        throw std::logic_error("logon cannot be initiated in the current state " + to_string(state()));
    }
    
    state(session_state::logging_on);

    if (logon_behaviour() != behaviour::initiator) {
        return;
    }

    send_logon();
}

void session::send_logon(bool reset_seq_num_flag)
{
    auto logon = message(true, {
        { FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Logon::MsgType },
        { FIX_5_0SP2::field::EncryptMethod::Tag, FIX_5_0SP2::field::EncryptMethod::None },
        { FIX_5_0SP2::field::HeartBtInt::Tag, heartbeat_interval() }
    });

    send(logon);
}

bool session::process_logon(const crocofix::message& message)
{
    auto reset_seq_num_flag = false;


    if (logon_behaviour() == behaviour::acceptor) {
        send_logon(reset_seq_num_flag);
    }

    send_post_logon_test_tequest();    
    
    return true;
}

void session::send_post_logon_test_tequest()
{
    if (test_request_delay() != 0) 
    {
    
    }
    else 
    {
        send_test_request();
    }
}

void session::send_test_request()
{
    m_expected_test_request_id = allocate_test_request_id();

    auto test_request = message(true, {
        { FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::TestRequest::MsgType },
        { FIX_5_0SP2::field::TestReqID::Tag, *m_expected_test_request_id }
    });

    send(test_request);
}

uint32_t session::allocate_test_request_id()
{
    return m_next_test_request_id++;
}

void session::process_test_request(const crocofix::message& test_request)
{
    auto testReqId = test_request.fields().try_get(FIX_5_0SP2::field::TestReqID::Tag);

    if (!testReqId) {
        std::cerr << "NO TEST REQ ID" << std::endl;
        return;
    }

    auto heartbeat = message(true, {
        { FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Heartbeat::MsgType },
        { FIX_5_0SP2::field::TestReqID::Tag, testReqId->value() }
    });

    send(heartbeat);
}

void session::process_heartbeat(const message& heartbeat)
{
    if (state() != session_state::logging_on && 
        state() != session_state::resetting && 
        state() != session_state::resending) 
    {
        return;
    }

    auto testReqId = heartbeat.fields().try_get(FIX_5_0SP2::field::TestReqID::Tag);

    if (testReqId) 
    {
        if (!m_expected_test_request_id) {
            // TODO
            return;
        }

        if (testReqId->value() != std::to_string(*m_expected_test_request_id)) {
            // TODO
            close();
            return;
        }
    }

    state(session_state::logged_on);
}

void session::send(message& message)
{
    message.fields().emplace_back(FIX_5_0SP2::field::SenderCompID::Tag, sender_comp_id());
    message.fields().emplace_back(FIX_5_0SP2::field::TargetCompID::Tag, target_comp_id());

    m_writer.write(message);
}

session_state session::state() const
{
    return m_state;
}
   
void session::state(session_state state)
{
    auto previous = m_state;
    m_state = state;
    state_changed(previous, m_state);
}

void session::ensure_options_are_mutable()
{
    if (state() != session_state::connected) {
        throw std::logic_error("session configuration cannot be changed while state = " + to_string(state()));
    }
}

behaviour session::logon_behaviour() const noexcept 
{ 
    return m_options.logon_behaviour(); 
}

void session::logon_behaviour(behaviour behaviour) noexcept
{
    ensure_options_are_mutable(); 
    m_options.logon_behaviour(behaviour); 
} 

const std::string& session::sender_comp_id() const noexcept
{
    return m_options.sender_comp_id();
}

void session::sender_comp_id(const std::string& sender_comp_id)
{
    ensure_options_are_mutable();
    m_options.sender_comp_id(sender_comp_id);
}    

const std::string& session::target_comp_id() const noexcept
{
    return m_options.target_comp_id();
}

void session::target_comp_id(const std::string& target_comp_id) noexcept
{
    ensure_options_are_mutable();
    m_options.target_comp_id(target_comp_id);
}

uint32_t session::heartbeat_interval() const noexcept 
{ 
    return m_options.heartbeat_interval(); 
}

void session::heartbeat_interval(uint32_t interval) noexcept 
{
    ensure_options_are_mutable();
    m_options.heartbeat_interval(interval); 
}

uint32_t session::test_request_delay() const noexcept
{
    return m_options.test_request_delay();
}

void session::test_request_delay(uint32_t delay) noexcept
{
    ensure_options_are_mutable();
    m_options.test_request_delay(delay);
}

}