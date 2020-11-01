#include <catch.hpp>
#include <libcrocofixdictionary/fix44_messages.hpp>
#include <libcrocofixdictionary/fix42_messages.hpp>

#include <algorithm>

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

    SECTION("Lookup message name") {
        REQUIRE(crocofix::FIX_4_4::messages().name_of_message("A") == "Logon");
        REQUIRE(crocofix::FIX_4_4::messages().name_of_message("ZZZZ") == "");
    }

    SECTION("test") {

        const auto quoteRequest = FIX_4_2::messages()["R"];

        auto prevClosePx = std::find_if(quoteRequest.fields().begin(),
                                     quoteRequest.fields().end(),
                                     [&](const auto& f) {
                                        return f.tag() == 140;
                                     });
        
        REQUIRE(prevClosePx != quoteRequest.fields().end());


        //assert.equal("PrevClosePx", orchestra.definitionOfField(140, FIX_4_2, quoteRequest)?.field.name);



        // const FIX_4_2 = orchestra.orchestrations[0];
        // const FIX_4_4 = orchestra.orchestrations[1];
        // const quoteRequest = orchestra.definitionOfMessage("R", FIX_4_2);
        // const quote = orchestra.definitionOfMessage("S", FIX_4_4);
        // orchestra.nameLookup = NameLookup.Strict;
        // assert.equal("PrevClosePx", orchestra.definitionOfField(140, FIX_4_2, quoteRequest)?.field.name);

    }

}