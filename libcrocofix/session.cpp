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
     m_reader.closed.connect([&](){ state(session_state::disconnected); });
     m_writer.closed.connect([&](){ state(session_state::disconnected); });
}

void session::open()
{
    logon();
}

void session::close()
{
    m_reader.close();
    m_writer.close();
    state(session_state::disconnected);
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

    if (!validate_begin_string(message)) {
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

    bool PossDupFlag = message.PossDupFlag();

    if (message.MsgType() == FIX_5_0SP2::message::SequenceReset::MsgType) {
        process_sequence_reset(message, PossDupFlag);
        return;
    }

    if (PossDupFlag) {
        warning("Ignoring PossDup Admin message with MsgType=" + message.MsgType());
        return;
    }

    if (message.MsgType() == FIX_5_0SP2::message::Logon::MsgType) {
        if (!process_logon(message)) {
            return;
        }
    }

    m_incoming_msg_seq_num = message.MsgSeqNum() + 1;

    if (message.MsgType() == FIX_5_0SP2::message::Heartbeat::MsgType) {
        process_heartbeat(message);
    }
    else if (message.MsgType() == FIX_5_0SP2::message::TestRequest::MsgType) {
        process_test_request(message);
    }
    else if(message.MsgType() == FIX_5_0SP2::message::ResendRequest::MsgType) {
        process_resend_request(message);
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

bool session::validate_begin_string(const crocofix::message& message)
{
    auto BeginString = message.fields().try_get(FIX_5_0SP2::field::BeginString::Tag);
    
    if (!BeginString) 
    {
        std::string text = "Received message without a BeginString"; 
        error(text);
        send_reject(message, text);
        close();
        return false;
    }

    if (BeginString->value() != begin_string()) 
    {
        std::string text = "Invalid BeginString, received " + BeginString->value() + " when expecting " + begin_string(); 
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

bool session::validate_sequence_numbers(const crocofix::message& message)
{
    if (sequence_number_is_high(message)) {
        return false;
    }

    if (sequence_number_is_low(message)) {
        return false;
    }

    return true;
}

bool session::sequence_number_is_high(const crocofix::message& message)
{
    auto MsgSeqNum = message.MsgSeqNum();

    if (MsgSeqNum > m_incoming_msg_seq_num) {
        request_resend(MsgSeqNum);
        return true;
    }
    
    return false;
}

bool session::sequence_number_is_low(const crocofix::message& message)
{
    auto MsgSeqNum = message.MsgSeqNum();

    if (MsgSeqNum < m_incoming_msg_seq_num) {
        std::string text = "MsgSeqNum too low, expecting " + std::to_string(incoming_msg_seq_num()) + " but received " + std::to_string(MsgSeqNum);
        error(text);
        send_logout(text);
        return true;
    }

    return false;
}

void session::request_resend(int received_msg_seq_num)
{
    information("Recoverable message sequence error, expected " + std::to_string(m_incoming_msg_seq_num) + 
                " received " + std::to_string(received_msg_seq_num) + " - initiating recovery");

    state(session_state::resending);

    m_incoming_resent_msg_seq_num = m_incoming_msg_seq_num;
    m_incoming_target_msg_seq_num = received_msg_seq_num;

    information("Requesting resend, BeginSeqNo " + std::to_string(m_incoming_msg_seq_num) + 
                " EndSeqNo " + std::to_string(received_msg_seq_num));

    auto resend_request = crocofix::message(true, {
        { FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::ResendRequest::MsgType },
        { FIX_5_0SP2::field::BeginSeqNo::Tag, m_incoming_msg_seq_num },
        { FIX_5_0SP2::field::EndSeqNo::Tag, received_msg_seq_num }
    });

    send(resend_request);
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
        // TODO - schedule a timer
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
    return m_outgoing_msg_seq_num++;
}

void session::process_test_request(const crocofix::message& test_request)
{
    auto testReqId = test_request.fields().try_get(FIX_5_0SP2::field::TestReqID::Tag);

    if (!testReqId) {
        std::string text = "TestRequest does not contain a TestReqID"; 
        send_reject(test_request, text);
        error(text);
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

void session::process_sequence_reset(const crocofix::message& sequence_reset, bool poss_dup)
{
    auto NewSeqNo = sequence_reset.fields().try_get(FIX_5_0SP2::field::NewSeqNo::Tag);

    if (!NewSeqNo) {
        std::string text = "SequenceReset does not contain a NewSeqNo field";
        error(text);
        send_reject(sequence_reset, text);
        return;
    }

    bool GapFill = sequence_reset.GapFillFlag();

    // TODO - this conversion needs a bounds check etc. Do the conversion in field and throw. Add a try/catch in the receive method
    if (std::stoi(NewSeqNo->value()) <= incoming_msg_seq_num()) {
        std::string text = "NewSeqNo is not greater than expected MsgSeqNum = " + std::to_string(incoming_msg_seq_num());
        error(text);
        send_reject(sequence_reset, text);
        return;
    }

    if (GapFill) {
                
        if (!validate_sequence_numbers(sequence_reset)) {
            return;
        }

        if ((m_next_expected_msg_seq_num && state() != session_state::logging_on) || 
            (!m_next_expected_msg_seq_num && state() != session_state::resending)) 
        {
            std::string text = "SequenceReset GapFill is not valid while not performing a resend";
            error(text);
            send_reject(sequence_reset, text);
            return;
        }
     }

}

void session::process_resend_request(const crocofix::message& resend_request)
{
    auto BeginSeqNo = resend_request.fields().try_get(FIX_5_0SP2::field::BeginSeqNo::Tag);

    if (!BeginSeqNo) {
        std::string text = "ResendRequest does not contain a BeginSeqNo field";
        error(text);
        send_reject(resend_request, text);
        return;
    }

    auto EndSeqNo = resend_request.fields().try_get(FIX_5_0SP2::field::EndSeqNo::Tag);

    if (!EndSeqNo) {
        std::string text = "ResendRequest does not contain a EndSeqNo field";
        error(text);
        send_reject(resend_request, text);
        return;
    }

}

void session::send(message& message, int options)
{
    if (options & encode_options::set_begin_string) {
        message.fields().set(FIX_5_0SP2::field::BeginString::Tag, begin_string());
    }

    message.fields().set(FIX_5_0SP2::field::SenderCompID::Tag, sender_comp_id());
    message.fields().set(FIX_5_0SP2::field::TargetCompID::Tag, target_comp_id());
    
    if (options & encode_options::set_msg_seq_num) {
        message.fields().set(FIX_5_0SP2::field::MsgSeqNum::Tag, allocate_outgoing_msg_seq_num());
    }

    message.fields().set(FIX_5_0SP2::field::SendingTime::Tag, timestamp_string(timestamp_format()));

    m_writer.write(message, options);
}

session_state session::state() const
{
    return m_state;
}
   
void session::state(session_state state)
{
    if (m_state != state) {
        auto previous = m_state;
        m_state = state;
        state_changed(previous, m_state);
    }
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

crocofix::timestamp_format session::timestamp_format() const noexcept
{
    return m_options.timestamp_format();
}

void session::timestamp_format(crocofix::timestamp_format format) noexcept
{
    m_options.timestamp_format(format);
}

uint32_t session::incoming_msg_seq_num() const
{
    return m_incoming_msg_seq_num;
}

uint32_t session::outgoing_msg_seq_num() const
{
    return m_outgoing_msg_seq_num;
}

}