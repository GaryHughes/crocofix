#include "session.hpp"
#include "probes.h"
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <functional>
#include <stdexcept>

namespace crocofix
{

session::session(reader& reader, writer& writer, scheduler& scheduler, const dictionary::orchestration& orchestration)
:   m_reader(reader),
    m_writer(writer),
    m_scheduler(scheduler),
    m_orchestration(orchestration)
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
    // It's cheaper and easier to just send heartbeats all the time once we are
    // logged on rather than tracking inactivity.
    stop_defibrillator();
    
    if (heartbeat_interval() == 0) {
        return;
    }
    
    m_heartbeat_timer_token = m_scheduler.schedule_repeating_callback(
        std::chrono::milliseconds(heartbeat_interval() * 1000),
        [&]() {
            defibrillate();
        }
    );
}

void session::stop_defibrillator()
{
    if (m_heartbeat_timer_token) {
        m_scheduler.cancel_callback(*m_heartbeat_timer_token);
        m_heartbeat_timer_token = std::nullopt;
    }
}

void session::defibrillate()
{
    if (state() != session_state::logged_on) {
        return;
    }

    auto heartbeat = message(true, {
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Heartbeat::MsgType )
    });

    send(heartbeat);
}

void session::on_message_read(crocofix::message& message)
{
    message_received(message);

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

    if (CROCOFIX_SESSION_MESSAGE_READ_ENABLED()) {
        CROCOFIX_SESSION_MESSAGE_READ(sender_comp_id().c_str(), 
                                      target_comp_id().c_str(), 
                                      message.MsgType().c_str());
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
    else {
        if (!validate_sequence_numbers(message)) {
            return;
        }
    }

    incoming_msg_seq_num(message.MsgSeqNum() + 1);

    if (message.MsgType() == FIX_5_0SP2::message::Heartbeat::MsgType) {
        process_heartbeat(message);
    }
    else if (message.MsgType() == FIX_5_0SP2::message::TestRequest::MsgType) {
        process_test_request(message);
    }
    else if (message.MsgType() == FIX_5_0SP2::message::ResendRequest::MsgType) {
        process_resend_request(message);
    }
    else if (message.MsgType() == FIX_5_0SP2::message::Logout::MsgType) {
        process_logout(message);
    }
}

bool session::validate_checksum(const crocofix::message& message)
{
    auto CheckSum = message.fields().try_get(FIX_5_0SP2::field::CheckSum::Tag);

    if (!CheckSum) 
    {
        std::string text = "Received message without a CheckSum"; 
        error(text);
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        close();
        return false;
    }

    auto calculated_checksum = message::format_checksum(message.calculate_checksum());

    if (CheckSum->value() != calculated_checksum) 
    {
        std::string text = "Received message with an invalid CheckSum, expected " + calculated_checksum + " received "  + CheckSum->value();
        error(text);
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::ValueIsIncorrect);
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
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        close();
        return false;
    }

    auto calculated_body_length = message.calculate_body_length();

    if (BodyLength->value() != std::to_string(calculated_body_length)) 
    {
        std::string text = "Received message with an invalid BodyLength, expected " + std::to_string(calculated_body_length) + " received " + BodyLength->value(); 
        error(text);
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::ValueIsIncorrect);
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
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        close();
        return false;
    }

    if (BeginString->value() != begin_string()) 
    {
        std::string text = "Invalid BeginString, received " + BeginString->value() + " when expecting " + begin_string(); 
        error(text);
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::ValueIsIncorrect);
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
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::CompIDProblem);
        close();
        return false;
    }

    auto SenderCompId = message.fields().try_get(FIX_5_0SP2::field::SenderCompID::Tag);

    if (!SenderCompId || SenderCompId->value() != target_comp_id()) 
    {
        const std::string text = "Received SenderCompID '" + (SenderCompId ? SenderCompId->value() : "") + "' when expecting '" + target_comp_id() + "'";
        error(text);
        send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::CompIDProblem);
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

    if (MsgSeqNum > incoming_msg_seq_num()) {
        request_resend(MsgSeqNum);
        return true;
    }
    
    return false;
}

bool session::sequence_number_is_low(const crocofix::message& message)
{
    auto MsgSeqNum = message.MsgSeqNum();

    if (MsgSeqNum < incoming_msg_seq_num()) {
        std::string text = "MsgSeqNum too low, expecting " + std::to_string(incoming_msg_seq_num()) + " but received " + std::to_string(MsgSeqNum);
        error(text);
        send_logout(text);
        return true;
    }

    return false;
}

void session::request_resend(int received_msg_seq_num)
{
    information("Recoverable message sequence error, expected " + std::to_string(incoming_msg_seq_num()) + 
                " received " + std::to_string(received_msg_seq_num) + " - initiating recovery");

    state(session_state::resending);

    m_incoming_resent_msg_seq_num = incoming_msg_seq_num();
    m_incoming_target_msg_seq_num = received_msg_seq_num;

    information("Requesting resend, BeginSeqNo " + std::to_string(incoming_msg_seq_num()) + 
                " EndSeqNo " + std::to_string(received_msg_seq_num));

    auto resend_request = crocofix::message(true, {
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::ResendRequest::MsgType ),
        field( FIX_5_0SP2::field::BeginSeqNo::Tag, incoming_msg_seq_num() ),
        field( FIX_5_0SP2::field::EndSeqNo::Tag, received_msg_seq_num )
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
    send_reject(message, text, FIX_5_0SP2::field::SessionRejectReason::Other);
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
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Logon::MsgType ),
        field( FIX_5_0SP2::field::EncryptMethod::Tag, FIX_5_0SP2::field::EncryptMethod::None ),
        field( FIX_5_0SP2::field::HeartBtInt::Tag, heartbeat_interval() )
    });

    if (reset_seq_num_flag) {
        logon.fields().set(FIX_5_0SP2::field::ResetSeqNumFlag::Tag, reset_seq_num_flag, set_operation::replace_first_or_append);
    }

    if (use_next_expected_msg_seq_num()) {
        logon.fields().set(FIX_5_0SP2::field::NextExpectedMsgSeqNum::Tag, incoming_msg_seq_num(), set_operation::replace_first_or_append);
    }

    send(logon);
}

bool session::process_logon(const crocofix::message& logon)
{
    auto reset_seq_num_flag = logon.ResetSeqNumFlag();

    if (!extract_heartbeat_interval(logon)) {
        return false;
    }

    m_logon_received = true;

    if (use_next_expected_msg_seq_num())
    {
        auto NextExpectedMsgSeqNum = logon.fields().try_get(FIX_5_0SP2::field::NextExpectedMsgSeqNum::Tag);

        if (!NextExpectedMsgSeqNum) {
            std::string text = "Logon does not contain NextExpectedMsgSeqNum";
            error(text);
            send_logout(text);
            return false;
        }

        uint32_t next_expected;

        try {
            next_expected = std::stoi(NextExpectedMsgSeqNum->value());
        }
        catch (std::exception& ex) {
            std::string text = NextExpectedMsgSeqNum->value() + " is not a valid value for NextExpectedMsgSeqNum";
            error(text);
            send_logout(text);
            return false;
        }

        if (next_expected > outgoing_msg_seq_num()) {
            std::string text = "NextExpectedMsgSeqNum too high, expecting " + std::to_string(outgoing_msg_seq_num()) + " but received " + std::to_string(next_expected);
            error(text);
            send_logout(text);
            return false;
        }

        if (next_expected + 1 < outgoing_msg_seq_num()) {
            std::string text = "NextExpectedMsgSeqNum too low, expecting " + std::to_string(outgoing_msg_seq_num()) + " but received " + std::to_string(next_expected) + " - performing resend";
            warning(text);
            state(session_state::resending);
            perform_resend(next_expected, outgoing_msg_seq_num());
        }
   
        if (logon_behaviour() == behaviour::acceptor) {
            send_logon();
        }

        send_post_logon_test_request();

        return true;
    }

    if (reset_seq_num_flag) 
    {
        if (logon.MsgSeqNum() != 1) {
            error("Invalid logon message, the ResetSeqNumFlag is set and the MsgSeqNum is not 1");
            return false;
        }

        if (state() == session_state::resetting) {
            send_test_request();
            return true;
        }

        information("Logon message received with ResetSeqNumFlag - resetting sequence numbers");
        reset();
        send_logon(true);
        send_post_logon_test_request();
        return true;
    }

    if (sequence_number_is_low(logon)) {
        return false;
    }

    if (logon_behaviour() == behaviour::acceptor) {
        send_logon(reset_seq_num_flag);
    }

    if (sequence_number_is_high(logon)) {
        return false;
    }

    send_post_logon_test_request();    
    
    return true;
}

void session::process_logout(const crocofix::message& logout)
{
    information("Closing session in response to Logout");
    close();
}

void session::reset()
{
    stop_defibrillator();
    state(session_state::resetting);
    outgoing_msg_seq_num(1);
    incoming_msg_seq_num(1);
}

bool session::extract_heartbeat_interval(const crocofix::message& logon)
{
    auto heartBtInt = logon.fields().try_get(FIX_5_0SP2::field::HeartBtInt::Tag);

    if (!heartBtInt) 
    {
        std::string text = "Logon message does not contain a HeartBtInt"; 
        error(text);
        send_reject(logon, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        send_logout(text);
        return false;
    }

    try
    {
        heartbeat_interval(std::stoi(heartBtInt->value()));
    }
    catch (std::exception& ex)
    {
        std::string text = heartBtInt->value() + " is not a valid numeric HeartBtInt";
        error(text);
        send_reject(logon, text, FIX_5_0SP2::field::SessionRejectReason::ValueIsIncorrect);
        send_logout(text);
        return false;
    }

    return true;
}

void session::send_reject(message message, const std::string& text, std::optional<dictionary::field_value> reason)
{
    auto reject = crocofix::message(true, {
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Reject::MsgType ),
        field( FIX_5_0SP2::field::RefSeqNum::Tag, message.MsgSeqNum() ),
        field( FIX_5_0SP2::field::Text::Tag, text )
    });

    if (reason && m_orchestration.is_field_defined(FIX_5_0SP2::field::SessionRejectReason::Tag))
    {
        reject.fields().set(FIX_5_0SP2::field::SessionRejectReason::Tag, std::string(reason->value()), set_operation::replace_first_or_append);          
    }

    send(reject); 
}

void session::send_logout(const std::string& text)
{
    auto logout = message(true, { 
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Logout::MsgType ),
        field( FIX_5_0SP2::field::Text::Tag, text )
    });

    send(logout);

    if (state() == session_state::logged_on) {
        stop_defibrillator();
        state(session_state::connected);
    }

    close();
}

void session::stop_test_request_timer()
{
    if (m_test_request_timer_token) {
        m_scheduler.cancel_callback(*m_test_request_timer_token);
        m_test_request_timer_token = std::nullopt;
    }
}

void session::send_post_logon_test_request()
{
    if (test_request_delay() != 0)
    {
        m_test_request_timer_token = m_scheduler.schedule_relative_callback(
            std::chrono::milliseconds(test_request_delay()),
            [&]() {
                m_test_request_timer_token = std::nullopt;
                send_test_request(); 
            }
        );
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
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::TestRequest::MsgType ),
        field( FIX_5_0SP2::field::TestReqID::Tag, *m_expected_test_request_id )
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
        send_reject(test_request, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        error(text);
        return;
    }

    auto heartbeat = message(true, {
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::Heartbeat::MsgType ),
        field( FIX_5_0SP2::field::TestReqID::Tag, testReqId->value() )
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
            warning("Received a Heartbeat with an unexpected TestReqID");
            return;
        }

        if (testReqId->value() != std::to_string(*m_expected_test_request_id)) {
            error("Received a Heartbeat with an incorrect TestReqID during Logon - disconnecting");
            close();
            return;
        }
    }

    state(session_state::logged_on);
    start_defibrillator();
}

void session::process_sequence_reset(const crocofix::message& sequence_reset, bool poss_dup)
{
    auto NewSeqNo_field = sequence_reset.fields().try_get(FIX_5_0SP2::field::NewSeqNo::Tag);

    if (!NewSeqNo_field) {
        std::string text = "SequenceReset does not contain a NewSeqNo field";
        error(text);
        send_reject(sequence_reset, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        return;
    }

    bool GapFill = sequence_reset.GapFillFlag();

    uint32_t NewSeqNo;

    try {
        NewSeqNo = std::stoi(NewSeqNo_field->value());
    }
    catch (std::exception& ex) {
        std::string text = NewSeqNo_field->value() + " is not a valid value for NewSeqNo";
        error(text);
        send_reject(sequence_reset, text, FIX_5_0SP2::field::SessionRejectReason::ValueIsIncorrect);
        return;
    }

    if (NewSeqNo <= incoming_msg_seq_num()) {
        std::string text = "NewSeqNo is not greater than expected MsgSeqNum = " + std::to_string(incoming_msg_seq_num());
        error(text);
        send_reject(sequence_reset, text, FIX_5_0SP2::field::SessionRejectReason::ValueIsIncorrect);
        return;
    }

    if (GapFill) {
                
        if (!validate_sequence_numbers(sequence_reset)) {
            return;
        }

        if ((use_next_expected_msg_seq_num() && state() != session_state::logging_on) || 
            (!use_next_expected_msg_seq_num() && state() != session_state::resending)) 
        {
            std::string text = "SequenceReset GapFill is not valid while not performing a resend";
            error(text);
            send_reject(sequence_reset, text, FIX_5_0SP2::field::SessionRejectReason::Other);
            return;
        }

        information("SeqenceReset (GapFill) received, NewSeqNo = " + std::to_string(NewSeqNo));
        
        m_incoming_resent_msg_seq_num = NewSeqNo;
        incoming_msg_seq_num(NewSeqNo);
     }
     else {
        m_incoming_resent_msg_seq_num = NewSeqNo;
        information("SeqenceReset (Reset) received, NewSeqNo = " + std::to_string(NewSeqNo));
    }

    if (state() == session_state::resending && m_incoming_resent_msg_seq_num >= m_incoming_target_msg_seq_num) {
        information("Resend complete");
        send_test_request();
        start_defibrillator();
    }
}

void session::process_resend_request(const crocofix::message& resend_request)
{
    stop_test_request_timer();

    auto BeginSeqNo = resend_request.fields().try_get(FIX_5_0SP2::field::BeginSeqNo::Tag);

    if (!BeginSeqNo) {
        std::string text = "ResendRequest does not contain a BeginSeqNo field";
        error(text);
        send_reject(resend_request, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        return;
    }

    auto EndSeqNo = resend_request.fields().try_get(FIX_5_0SP2::field::EndSeqNo::Tag);

    if (!EndSeqNo) {
        std::string text = "ResendRequest does not contain a EndSeqNo field";
        error(text);
        send_reject(resend_request, text, FIX_5_0SP2::field::SessionRejectReason::RequiredTagMissing);
        return;
    }

    uint32_t begin = std::stoi(BeginSeqNo->value());
    uint32_t end = std::stoi(EndSeqNo->value());

    information("Performing resend from BeginSeqNo = " + std::to_string(begin) + " to EndSeqNo " + std::to_string(end));

    if (begin_string() == "FIX.4.0" || begin_string() == "FIX.4.1") {
        if (end == 9999) {
            end = 0;
        }
    }

    perform_resend(begin, end);

    send_post_logon_test_request();
}

void session::send_gap_fill(uint32_t msg_seq_num, int new_seq_no)
{
    auto sequence_reset = message(true, {
        field( FIX_5_0SP2::field::MsgType::Tag, FIX_5_0SP2::message::SequenceReset::MsgType ),
        field( FIX_5_0SP2::field::MsgSeqNum::Tag, msg_seq_num ),
        field( FIX_5_0SP2::field::GapFillFlag::Tag, true ),
        field( FIX_5_0SP2::field::PossDupFlag::Tag, true ),
        field( FIX_5_0SP2::field::NewSeqNo::Tag, new_seq_no )
    });

    send(sequence_reset, encode_options::standard & ~encode_options::set_msg_seq_num);
}

void session::perform_resend(uint32_t begin_msg_seq_num, uint32_t end_msg_seq_num)
{
    // TODO
    send_gap_fill(begin_msg_seq_num, outgoing_msg_seq_num());
}

void session::send(message& message, int options)
{
    if (message.MsgType() == FIX_5_0SP2::message::Logon::MsgType && message.ResetSeqNumFlag())
    {
        state(session_state::resetting);
        outgoing_msg_seq_num(1);
        incoming_msg_seq_num(1);
    }

    if (options & encode_options::set_begin_string) {
        message.fields().set(FIX_5_0SP2::field::BeginString::Tag, begin_string());
    }

    message.fields().set(FIX_5_0SP2::field::SenderCompID::Tag, sender_comp_id());
    message.fields().set(FIX_5_0SP2::field::TargetCompID::Tag, target_comp_id());
    
    if (options & encode_options::set_msg_seq_num) {
        message.fields().set(FIX_5_0SP2::field::MsgSeqNum::Tag, allocate_outgoing_msg_seq_num());
    }

    message.fields().set(FIX_5_0SP2::field::SendingTime::Tag, timestamp_string(timestamp_format()));

    if (options & encode_options::set_checksum) {
        // message::encode will give this a real value
        message.fields().set(FIX_5_0SP2::field::CheckSum::Tag, "", set_operation::replace_first_or_append);    
    }

    if (CROCOFIX_SESSION_MESSAGE_WRITE_ENABLED()) {
        CROCOFIX_SESSION_MESSAGE_WRITE(sender_comp_id().c_str(), 
                                       target_comp_id().c_str(), 
                                       message.MsgType().c_str());
    }  

    m_writer.write(message, options);

    message_sent(message);
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

uint32_t session::incoming_msg_seq_num() const noexcept
{
    return m_incoming_msg_seq_num;
}

void session::incoming_msg_seq_num(uint32_t msg_seq_num) noexcept
{
    m_incoming_msg_seq_num = msg_seq_num;
}

uint32_t session::outgoing_msg_seq_num() const noexcept
{
    return m_outgoing_msg_seq_num;
}

void session::outgoing_msg_seq_num(uint32_t msg_seq_num) noexcept
{
    // TODO - throw if not connected/disconnected
    m_outgoing_msg_seq_num = msg_seq_num;
}

bool session::use_next_expected_msg_seq_num() const noexcept
{
    return m_options.use_next_expected_msg_seq_num();
}

void session::use_next_expected_msg_seq_num(bool value) noexcept
{
    m_options.use_next_expected_msg_seq_num(value);
}

}