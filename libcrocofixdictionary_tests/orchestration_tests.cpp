#include <catch2/catch_all.hpp>
#include <libcrocofixdictionary/fix44_orchestration.hpp>

using namespace crocofix;

TEST_CASE("orchestration", "orchestration") {

    SECTION("Message Definitions") {

        auto orchestration = crocofix::FIX_4_4::orchestration();

        REQUIRE(orchestration.messages().size() == 93);

        auto heartbeat = orchestration.messages()[0];

        REQUIRE(heartbeat.name() == "Heartbeat");
        REQUIRE(heartbeat.msg_type() == "0");
        REQUIRE(heartbeat.category() == "Session");
        REQUIRE(heartbeat.pedigree().added() == "FIX.2.7");        
        REQUIRE(heartbeat.synopsis() == "The Heartbeat monitors the status of the communication link and identifies when the last of a string of messages was not received.");
    }

    SECTION("MsgType lookup with valid MsgType succeeds") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        const auto& executionReport = orchestration.messages()["8"];
        REQUIRE(executionReport.name() == "ExecutionReport");
    }

    SECTION("MsgTpe lookup with invalid MsgType throws") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        REQUIRE_THROWS(orchestration.messages()["XXX"]);
    }

    SECTION("Message fields") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        auto heartbeat = orchestration.messages()[0];
        REQUIRE(heartbeat.fields().size() == 34);
    }

    SECTION("Lookup message name") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        REQUIRE(orchestration.messages().name_of_message("A") == "Logon");
        REQUIRE(orchestration.messages().name_of_message("ZZZZ").empty());
    }

     SECTION("Version Field Definitions") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        REQUIRE(orchestration.fields()[0].is_valid() == false);
        REQUIRE(orchestration.fields().size() == 913);
        REQUIRE(orchestration.fields()[54].is_valid() == true);
        REQUIRE(orchestration.fields()[54].tag() == 54);

    }

    SECTION("Lookup field name") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        REQUIRE(orchestration.fields().name_of_field(100) == "ExDestination");
        REQUIRE(orchestration.fields().name_of_field(999999).empty());
    }

    SECTION("Lookup field value") {
        auto orchestration = crocofix::FIX_4_4::orchestration();
        REQUIRE(orchestration.fields().name_of_value(18, "G") == "AllOrNone");
        REQUIRE(orchestration.fields().name_of_value(999999, "1").empty());
        REQUIRE(orchestration.fields().name_of_value(999999, "54").empty());
    }

}