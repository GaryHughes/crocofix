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

TEST_CASE_METHOD(crocofix::session_fixture, "Logon with wrong SenderCompID")
{
    initiator.sender_comp_id("WRONG");

    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(initiator.state() == crocofix::session_state::connected);

    acceptor.open();
    initiator.open();

    REQUIRE(sent_from_initiator(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }
    }));

    REQUIRE(received_at_acceptor(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }
    }));

    REQUIRE(acceptor_state_change(crocofix::session_state::logging_on));
    REQUIRE(initiator_state_change(crocofix::session_state::logging_on));

    REQUIRE(sent_from_acceptor(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "Received SenderCompID 'WRONG' when expecting 'INITIATOR'" }
    }));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "Received SenderCompID 'WRONG' when expecting 'INITIATOR'"}
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "Logon with wrong TargetCompID")
{
    initiator.target_comp_id("WRONG");

    REQUIRE(acceptor.state() == crocofix::session_state::connected);
    REQUIRE(initiator.state() == crocofix::session_state::connected);

    acceptor.open();
    initiator.open();

     REQUIRE(sent_from_initiator(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }
    }));

    REQUIRE(received_at_acceptor(fix::message::Logon::MsgType, {
        { fix::field::SenderCompID::Tag, initiator.sender_comp_id() },
        { fix::field::TargetCompID::Tag, initiator.target_comp_id() }
    }));

    REQUIRE(acceptor_state_change(crocofix::session_state::logging_on));
    REQUIRE(initiator_state_change(crocofix::session_state::logging_on));

    REQUIRE(sent_from_acceptor(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "Received TargetCompID 'WRONG' when expecting 'ACCEPTOR'" }
    }));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 1 },
        { fix::field::Text::Tag, "Received TargetCompID 'WRONG' when expecting 'ACCEPTOR'"}
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "Invalid CheckSum")
{
    perform_default_logon_sequence();

    send_from_initiator(
        fix::message::TestRequest::MsgType, {
            { fix::field::MsgType::Tag, fix::message::TestRequest::MsgType },
            { fix::field::CheckSum::Tag, 666 }
        },
        crocofix::encode_options::standard & ~crocofix::encode_options::set_checksum
    );

    REQUIRE(received_at_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::CheckSum::Tag, 666 }
    }));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 4 },
        // We can't test the Text because it contains the CheckSum which varies with SendingTime.
    }));
}

// We can't write a missing CheckSum test because message decode uses it for the delimiter.

TEST_CASE_METHOD(crocofix::session_fixture, "Invalid BodyLength")
{
    perform_default_logon_sequence();

    send_from_initiator(
        fix::message::TestRequest::MsgType, {
            { fix::field::MsgType::Tag, fix::message::TestRequest::MsgType },
            { fix::field::BodyLength::Tag, 666 }
        },
        crocofix::encode_options::standard & ~crocofix::encode_options::set_body_length
    );

    REQUIRE(received_at_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::BodyLength::Tag, 666 }
    }));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 4 },
        { fix::field::Text::Tag, "Received message with an invalid BodyLength, expected 39 received 666" }
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "Missing BodyLength")
{
    perform_default_logon_sequence();

    send_from_initiator(
        fix::message::TestRequest::MsgType, {
            { fix::field::MsgType::Tag, fix::message::TestRequest::MsgType }
        },
        crocofix::encode_options::standard & ~crocofix::encode_options::set_body_length,
        { fix::field::BodyLength::Tag }
    );

    REQUIRE(received_at_acceptor(fix::message::TestRequest::MsgType));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 4 },
        { fix::field::Text::Tag, "Received message without a BodyLength" }
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "Wrong BeginString")
{
    perform_default_logon_sequence();

    send_from_initiator(
        fix::message::TestRequest::MsgType, {
            { fix::field::MsgType::Tag, fix::message::TestRequest::MsgType },
            { fix::field::BeginString::Tag, "WRONG" }
        },
        crocofix::encode_options::standard & ~crocofix::encode_options::set_begin_string
    );

    REQUIRE(received_at_acceptor(fix::message::TestRequest::MsgType, {
        { fix::field::BeginString::Tag, "WRONG" }
    }));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 4 },
        { fix::field::Text::Tag, "Invalid BeginString, received WRONG when expecting " + initiator.begin_string() }
    }));
}

TEST_CASE_METHOD(crocofix::session_fixture, "Missing BeginString")
{
    perform_default_logon_sequence();
    
    send_from_initiator(
        fix::message::TestRequest::MsgType, {
            { fix::field::MsgType::Tag, fix::message::TestRequest::MsgType }
        },
        crocofix::encode_options::standard & ~crocofix::encode_options::set_begin_string,
        { fix::field::BeginString::Tag }
    );

    REQUIRE(received_at_acceptor(fix::message::TestRequest::MsgType));

    REQUIRE(received_at_initiator(fix::message::Reject::MsgType, {
        { fix::field::RefSeqNum::Tag, 4 },
        { fix::field::Text::Tag, "Received message without a BeginString" }
    }));
}
