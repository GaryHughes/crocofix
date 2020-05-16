#include "session_fixture.hpp"
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <catch.hpp>

namespace crocofix
{

namespace fix = crocofix::FIX_5_0SP2;

session_fixture::session_fixture()
:   initiator_reader(initiator_incoming_messages),
    initiator_writer(acceptor_reader, initiator_outgoing_messages),
    initiator(initiator_reader, initiator_writer),
    acceptor_reader(acceptor_incoming_messages),
    acceptor_writer(initiator_reader, acceptor_outgoing_messages),
    acceptor(acceptor_reader, acceptor_writer)
{
    acceptor.logon_behaviour(crocofix::behaviour::acceptor);
}

void session_fixture::perform_default_logon_sequence()
{
    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(initiator.state() == crocofix::session_state::connected);

    acceptor.open();
    initiator.open();

    REQUIRE(sent_from_initiator(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }    
    }));

    REQUIRE(receive_at_acceptor(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }    
    }));

    REQUIRE(sent_from_acceptor(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() }    
    }));

    REQUIRE(receive_at_initiator(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() }    
    }));

    REQUIRE(acceptor.state() == crocofix::session_state::logging_on);
    REQUIRE(initiator.state() == crocofix::session_state::logging_on);

    REQUIRE(sent_from_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TestReqID::Tag, 0 }    
    }));

    REQUIRE(sent_from_initiator(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
        { fix::field::TestReqID::Tag, 0 }    
    }));

    REQUIRE(receive_at_initiator(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TestReqID::Tag, 0 } 
    }));

    REQUIRE(receive_at_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
        { fix::field::TestReqID::Tag, 0 }
    }));

    REQUIRE(sent_from_acceptor(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
    }));

    REQUIRE(receive_at_initiator(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
    }));

    REQUIRE(sent_from_initiator(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
    }));

    REQUIRE(receive_at_acceptor(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
    }));

    REQUIRE(acceptor.state() == crocofix::session_state::logged_on);
    REQUIRE(initiator.state() == crocofix::session_state::logged_on);
}

bool session_fixture::sent_from_initiator(
    const std::string& msg_type, 
    const std::initializer_list<crocofix::field> fields,
    const std::chrono::milliseconds timeout)
{
    return expect(initiator_outgoing_messages, msg_type, fields, timeout);
}

bool session_fixture::receive_at_acceptor(
    const std::string& msg_type, 
    const std::initializer_list<crocofix::field> fields,
    const std::chrono::milliseconds timeout)
{
    return expect(acceptor_incoming_messages, msg_type, fields, timeout);
}

bool session_fixture::sent_from_acceptor(
    const std::string& msg_type, 
    const std::initializer_list<crocofix::field> fields,
    const std::chrono::milliseconds timeout)
{
    return expect(acceptor_outgoing_messages, msg_type, fields, timeout);
}

bool session_fixture::receive_at_initiator(
    const std::string& msg_type, 
    const std::initializer_list<crocofix::field> fields,
    const std::chrono::milliseconds timeout)
{
    return expect(initiator_incoming_messages, msg_type, fields, timeout);
}

bool session_fixture::expect(
    blocking_queue<message>& messages,
    const std::string& msg_type, 
    const std::initializer_list<crocofix::field> fields,
    const std::chrono::milliseconds timeout)
{
    crocofix::message message;
    
    if (!messages.try_dequeue(message, timeout)) {
        UNSCOPED_INFO("failed to dequeue message with MsgType=" + msg_type);
        return false;
    }

    if (message.MsgType() != msg_type) {
        UNSCOPED_INFO("dequeued a message with MsgType=" + message.MsgType() + " when expecting MsgType=" + msg_type);
        return false;
    }

    // We care that all expected fields are present and in the correct order.
    // We do not care if there are other fields present as well.
    auto current = message.fields().begin();

    for (const auto& expected : fields)
    {
        std::optional<crocofix::field> actual;

        for (; current != message.fields().end(); ++current)
        {
            if (current->tag() == expected.tag()) {
                actual = *current;
                break;
            }
        }

        if (!actual) {
            UNSCOPED_INFO("message does not contain expected field with tag=" + std::to_string(expected.tag()));
            return false;
        }

        if (actual->value() != expected.value()) {
            UNSCOPED_INFO("message contains expected field with tag=" + std::to_string(expected.tag()) + 
                          " but it contains the value '" + actual->value() << "' when expecting '" + expected.value() + "'");
            return false;
        } 
    }
    
    return true;
}

}
