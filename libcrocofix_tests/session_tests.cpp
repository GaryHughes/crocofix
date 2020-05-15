#include <catch.hpp>
#include "session_fixture.hpp"

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



