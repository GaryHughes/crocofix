#include <catch.hpp>
#include <libcrocofix/session.hpp>
#include <iostream>

using namespace crocofix;

TEST_CASE("Session State", "[session state]") {

    SECTION("to_string") 
    {
        REQUIRE(crocofix::to_string(session_state::connected) == "connected");
        REQUIRE(crocofix::to_string(session_state::logging_on) == "loggin_on");
        REQUIRE(crocofix::to_string(session_state::resending) == "resending");
        REQUIRE(crocofix::to_string(session_state::logged_on) == "logged_on");
        REQUIRE(crocofix::to_string(session_state::resetting) == "resetting");
        REQUIRE(crocofix::to_string(static_cast<session_state>(99)) == "unknown");
    }

}
