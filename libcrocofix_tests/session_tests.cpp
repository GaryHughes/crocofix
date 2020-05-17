#include <catch.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include "session_fixture.hpp"

namespace fix = crocofix::FIX_5_0SP2;

TEST_CASE_METHOD(crocofix::session_fixture, "default state") 
{
    REQUIRE(initiator.state() == crocofix::session_state::connected);
    REQUIRE(initiator.logon_behaviour() == crocofix::behaviour::initiator);

    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(acceptor.logon_behaviour() == crocofix::behaviour::acceptor);
}

TEST_CASE_METHOD(crocofix::session_fixture, "logon")
{
    perform_default_logon_sequence();
}

TEST_CASE_METHOD(crocofix::session_fixture, "Logon with no HeartBtInt")
{
    // disable the default logon so we can send it manually
    initiator.logon_behaviour(crocofix::behaviour::acceptor);

    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(initiator.state() == crocofix::session_state::connected);

    acceptor.open();
    initiator.open();

    REQUIRE(acceptor_state_change(crocofix::session_state::logging_on));
    REQUIRE(initiator_state_change(crocofix::session_state::logging_on));

    send_from_initiator(fix::message::Logon::MsgType);
    REQUIRE(received_at_acceptor(fix::message::Logon::MsgType));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "Logon message does not contain a HeartBtInt" },
        // { fix::field::SessionRejectReason::Tag, fix::field::SessionRejectReason::RequiredTagMissing }
    }));

    REQUIRE(received_at_initiator(fix::message::Logout::MsgType, {
        { fix::field::Text::Tag, "Logon message does not contain a HeartBtInt" }
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "Logon with invalid HeartBtInt")
{
     // disable the default logon so we can send it manually
    initiator.logon_behaviour(crocofix::behaviour::acceptor);

    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(initiator.state() == crocofix::session_state::connected);

    acceptor.open();
    initiator.open();

    REQUIRE(acceptor_state_change(crocofix::session_state::logging_on));
    REQUIRE(initiator_state_change(crocofix::session_state::logging_on));

    send_from_initiator(fix::message::Logon::MsgType, {
        { fix::field::HeartBtInt::Tag, "XYZ" }
    });
    REQUIRE(received_at_acceptor(fix::message::Logon::MsgType));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "XYZ is not a valid numeric HeartBtInt" },
        // { fix::field::SessionRejectReason::Tag, fix::field::SessionRejectReason::ValueIsIncorrect }
    }));

    REQUIRE(received_at_initiator(fix::message::Logout::MsgType, {
        { fix::field::Text::Tag, "XYZ is not a valid numeric HeartBtInt" }
    }));
 }

TEST_CASE_METHOD(crocofix::session_fixture, "Test HeartBtInt extracted from Logon")
{
    acceptor.heartbeat_interval(30);
    initiator.heartbeat_interval(10);

    perform_default_logon_sequence();

    REQUIRE(acceptor.heartbeat_interval() == 10);
}

TEST_CASE_METHOD(crocofix::session_fixture, "TestRequest")
{
    perform_default_logon_sequence();

    send_from_initiator(fix::message::TestRequest::MsgType, {
        { fix::field::TestReqID::Tag, "TEST" }
    });

    REQUIRE(received_at_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::TestReqID::Tag, "TEST" }
    }));

    REQUIRE(received_at_initiator(fix::message::Heartbeat::MsgType, {
        { fix::field::TestReqID::Tag, "TEST" }
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "First message not Logon")
{
     // disable the default logon so we can send it manually
    initiator.logon_behaviour(crocofix::behaviour::acceptor);
    
    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(initiator.state() == crocofix::session_state::connected);

    acceptor.open();
    initiator.open();

    REQUIRE(acceptor_state_change(crocofix::session_state::logging_on));
    REQUIRE(initiator_state_change(crocofix::session_state::logging_on));

    send_from_initiator(fix::message::NewOrderSingle::MsgType);
    REQUIRE(received_at_acceptor(fix::message::NewOrderSingle::MsgType));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "First message is not a Logon" }
    }));

    REQUIRE(received_at_initiator(fix::message::Logout::MsgType, {
        { fix::field::Text::Tag, "First message is not a Logon" }
    }));

    // TODO - add a test for logon after logout
}

