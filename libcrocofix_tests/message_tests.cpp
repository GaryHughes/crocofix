#include <catch.hpp>
#include <libcrocofix/message.hpp>

using namespace crocofix;

TEST_CASE("Message Decoding", "[message]") {

    SECTION("Decode a complete message") {
        std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(text);
        REQUIRE(complete);
        REQUIRE(consumed == text.length());
        REQUIRE(message.fields().size() == 18);
    }

    SECTION("Decode a complete message in two pieces aligned on a field boundary") {
        std::string one = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u0001";
        std::string two = "54=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto one_result = message.decode(one);
        REQUIRE(!one_result.complete);
        REQUIRE(one_result.consumed == one.length());
        auto two_result = message.decode(two);
        REQUIRE(two_result.complete);
        REQUIRE(two_result.consumed == two.length());
        REQUIRE(message.fields().size() == 18);
    }

    SECTION("Decode a complete message in two pieces not aligned on a field boundary") {
        std::string one = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=B";
        std::string two = "55=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto one_result = message.decode(one);
        REQUIRE(!one_result.complete);
        REQUIRE(one_result.consumed == one.length() - std::string("55=B").length());
        auto two_result = message.decode(two);
        REQUIRE(two_result.complete);
        REQUIRE(two_result.consumed == two.length());
        REQUIRE(message.fields().size() == 18);
    }
  
    SECTION("Invalid tag throws") {
        crocofix::message message;
        REQUIRE_THROWS_AS(message.decode("A=FIX.4.4"), std::out_of_range);
    }

}