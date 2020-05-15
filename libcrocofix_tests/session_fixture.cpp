#include "session_fixture.hpp"
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <catch.hpp>

namespace crocofix
{

namespace fix = crocofix::FIX_5_0SP2;

session_fixture::session_fixture()
:   initiator(initiator_reader, initiator_writer),
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

    sent_from_initiator(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }    
    });

    receive_at_acceptor(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }    
    });

    sent_from_acceptor(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() }    
    });

    receive_at_initiator(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() }    
    });

    REQUIRE(acceptor.state() == crocofix::session_state::logging_on);
    REQUIRE(initiator.state() == crocofix::session_state::logging_on);

    sent_from_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TestReqID::Tag, 0 }    
    });

    sent_from_initiator(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
        { fix::field::TestReqID::Tag, 0 }    
    });

    receive_at_initiator(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TestReqID::Tag, 0 } 
    });

    receive_at_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
        { fix::field::TestReqID::Tag, 0 }
    });

    sent_from_acceptor(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
    });

    receive_at_initiator(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.target_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.sender_comp_id() },
    });

    sent_from_initiator(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
    });

    receive_at_acceptor(fix::message::Heartbeat::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() },
    });

    REQUIRE(acceptor.state() == crocofix::session_state::logged_on);
    REQUIRE(initiator.state() == crocofix::session_state::logged_on);
}

void session_fixture::sent_from_initiator(const std::string& msg_type, std::initializer_list<crocofix::field> fields)
{

}

void session_fixture::receive_at_acceptor(const std::string& msg_type, std::initializer_list<crocofix::field> fields)
{

}

void session_fixture::sent_from_acceptor(const std::string& msg_type, std::initializer_list<crocofix::field> fields)
{

}

void session_fixture::receive_at_initiator(const std::string& msg_type, std::initializer_list<crocofix::field> fields)
{

}

}
