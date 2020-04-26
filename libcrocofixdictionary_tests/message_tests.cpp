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
    }

}