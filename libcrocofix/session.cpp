#include "session.hpp"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <functional>
#include <stdexcept>


#include <iostream>

namespace crocofix
{

session::session(reader& reader, writer& writer, scheduler& scheduler)
:   m_reader(reader),
    m_writer(writer),
    m_scheduler(scheduler)
{
     m_reader.read_async([=](crocofix::message& message) { on_message_read(message); });
}    

void session::open()
{
    logon();
}

void session::close()
{
    m_reader.close();
    m_writer.close();
}

void session::start_defibrillator()
{

}

void session::stop_defibrillator()
{

}

void session::on_message_read(crocofix::message& message)
{
    if (!validate_checksum(message)) {
        return;
    }

    if (!validate_body_length(message)) {
        return;
    }

    if (!validate_comp_ids(message)) {
        return;
    }

    if (!validate_first_message(message)) {
        return;
    }

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

bool session::validate_checksum(const crocofix::message& message)
{
    auto CheckSum = message.fields().try_get(FIX_5_0SP2::field::CheckSum::Tag);

    if (!CheckSum) 
    {
        std::string text = "Received message without a CheckSum"; 
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    auto calculated_checksum = message::format_checksum(message.calculate_checksum());

    if (CheckSum->value() != calculated_checksum) 
    {
        std::string text = "Received message with an invalid CheckSum, expected " + calculated_checksum + " received "  + CheckSum->value();
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    return true;
}

bool session::validate_body_length(const crocofix::message& message)
{
    auto BodyLength = message.fields().try_get(FIX_5_0SP2::field::BodyLength::Tag);

    if (!BodyLength) 
    {
        std::string text = "Received message without a BodyLength";
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    auto calculated_body_length = message.calculate_body_length();

    if (BodyLength->value() != std::to_string(calculated_body_length)) 
    {
        std::string text = "Received message with an invalid BodyLength, expected " + std::to_string(calculated_body_length) + " received " + BodyLength->value(); 
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    return true;
}

bool session::validate_comp_ids(const crocofix::message& message)
{
    auto TargetCompId = message.fields().try_get(FIX_5_0SP2::field::TargetCompID::Tag);

    if (!TargetCompId || TargetCompId->value() != sender_comp_id())
    {
        const std::string text = "Received TargetCompID '" + (TargetCompId ? TargetCompId->value() : "") + "' when expecting '" + sender_comp_id() + "'";
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    auto SenderCompId = message.fields().try_get(FIX_5_0SP2::field::SenderCompID::Tag);

    if (!SenderCompId || SenderCompId->value() != target_comp_id()) 
    {
        const std::string text = "Received SenderCompID '" + (SenderCompId ? SenderCompId->value() : "") + "' when expecting '" + target_comp_id() + "'";
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    return true;
}

bool session::validate_first_message(const crocofix::message& message)
{
    if (logon_behaviour() == behaviour::initiator) {
        return true;
    }

    if (m_logon_received) {
        return true;
    }

    if (message.MsgType() == FIX_5_0SP2::message::Logon::MsgType) {
        return true;
    } 

    if (message.MsgType() == FIX_5_0SP2::message::Reject::MsgType ||
        message.MsgType() == FIX_5_0SP2::message::Logout::MsgType)
    {
        return true;
    }

    const std::string text = "First message is not a Logon";
    error(text);
    send_reject(message, text);
    send_logout(text);
    
    return false;
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

bool session::process_logon(const crocofix::message& logon)
{
    auto reset_seq_num_flag = false;

    if (!extract_heartbeat_interval(logon)) {
        return false;
    }

    m_logon_received = true;

    if (logon_behaviour() == behaviour::acceptor) {
        send_logon(reset_seq_num_flag);
    }

    send_post_logon_test_tequest();    
    
    return true;
}

bool session::extract_heartbeat_interval(const crocofix::message& logon)
{
    auto heartBtInt = logon.fields().try_get(FIX_5_0SP2::field::HeartBtInt::Tag);

    if (!heartBtInt) 
    {
        auto text = "Logon message does not contain a HeartBtInt"; 
        error(text);
        send_reject(logon, text);
        send_logout(text);
        return false;
    }

    try
    {
        heartbeat_interval(std::stoi(heartBtInt->value()));
    }
    catch (std::exception& ex)
    {
        auto text = heartBtInt->value() + " is not a valid numeric HeartBtInt";
        error(text);
        send_reject(logon, text);
        send_logout(text);
        return false;
    }

    return true;
}

void session::send_reject(message message, const std::string& text)
{
    auto reject = crocofix::message(true, {
        { FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Reject::MsgType },
        { FIX_5_0SP2::field::RefSeqNum::Tag, message.MsgSeqNum() },
        { FIX_5_0SP2::field::Text::Tag, text }
    });

    // TODO: SessionRejectReason 
    
    send(reject); 
}

void session::send_logout(const std::string& text)
{
    auto logout = message(true, { 
        { FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Logout::MsgType },
        { FIX_5_0SP2::field::Text::Tag, text }
    });

    send(logout);

    if (state() == session_state::logged_on) {
        stop_defibrillator();
        state(session_state::connected);
    }

    close();
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

uint32_t session::allocate_outgoing_msg_seq_num()
{
    return m_next_outgoing_msg_seq_num++;
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

void session::send(message& message, int options)
{
    message.fields().set(FIX_5_0SP2::field::BeginString::Tag, begin_string());
    message.fields().set(FIX_5_0SP2::field::SenderCompID::Tag, sender_comp_id());
    message.fields().set(FIX_5_0SP2::field::TargetCompID::Tag, target_comp_id());
    message.fields().set(FIX_5_0SP2::field::MsgSeqNum::Tag, allocate_outgoing_msg_seq_num());

    m_writer.write(message, options);
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

const std::string& session::begin_string() const noexcept
{
    return m_options.begin_string();
}

void session::begin_string(const std::string& begin_string)
{
    ensure_options_are_mutable();
    m_options.begin_string(begin_string);
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
    // TODO - validate and start/stop defib if required
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