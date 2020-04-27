#include <catch.hpp>
#include <libcrocofixdictionary/fix44_messages.hpp>

using namespace crocofix;

TEST_CASE("message", "[message]") {

    SECTION("Message Definitions") {

        REQUIRE(crocofix::FIX_4_4::messages().size() == 93);

        auto heartbeat = crocofix::FIX_4_4::messages()[0];

        REQUIRE(heartbeat.name() == "Heartbeat");
        REQUIRE(heartbeat.msg_type() == "0");
        REQUIRE(heartbeat.category() == "Session");
        REQUIRE(heartbeat.added() == "FIX.2.7");        
        REQUIRE(heartbeat.synopsis() == "The Heartbeat monitors the status of the communication link and identifies when the last of a string of messages was not received.");
    }

    SECTION("MsgType lookup with valid MsgType succeeds") {
        const auto& executionReport = crocofix::FIX_4_4::messages()["8"];
        REQUIRE(executionReport.name() == "ExecutionReport");
    }

    SECTION("MsgTpe lookup with invalid MsgType throws") {
        REQUIRE_THROWS(crocofix::FIX_4_4::messages()["XXX"]);
    }

    SECTION("Message fields") {

        auto heartbeat = crocofix::FIX_4_4::messages()[0];

        REQUIRE(heartbeat.fields().size() == 30);

    }

}