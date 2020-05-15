#include <catch.hpp>
#include <libcrocofix/session.hpp>
#include "test_reader.hpp"
#include "test_writer.hpp"

TEST_CASE("Session", "[session]") {

    crocofix::test_reader reader;
    crocofix::test_writer writer;
    crocofix::session session(reader, writer);

    SECTION("Default State") 
    {
        REQUIRE(session.state() == crocofix::session_state::connected);
        REQUIRE(session.logon_behaviour() == crocofix::behaviour::initiator);
    }


}