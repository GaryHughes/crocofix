#include <catch.hpp>
#include <libcrocofix/message.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using namespace crocofix;

TEST_CASE("Message", "[message]") {

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

    SECTION("MsgType lookup throws for a message with no MsgType") {
        std::string text = "8=FIX.4.4\u00019=149\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE_THROWS(message.MsgType());
    }

    SECTION("MsgType lookup") {
        std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE(message.MsgType() == "D");
    }

    SECTION("is_admin is false for a non admin message") {
        std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE(message.is_admin() == false);        
    }

    SECTION("is_admin is true for an admin message") {
        std::string text = "8=FIX.4.4\u00019=149\u000135=A\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE(message.is_admin() == true);        
    }

    SECTION("encode a message") {
        std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(expected);

        std::array<char, 1024> buffer;
        auto result = message.encode(gsl::span(buffer.data(), buffer.size()));
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual == expected);
    }

    SECTION("encode does not add CheckSum if it is not present") {
        std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u0001";
        crocofix::message message;
        message.decode(expected);

        std::array<char, 1024> buffer;
        auto result = message.encode(gsl::span(buffer.data(), buffer.size()));
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual == expected);
    }

    SECTION("encode does not add BodyLength if it is not present") {
        std::string expected = "8=FIX.4.4\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(expected);

        std::array<char, 1024> buffer;
        auto result = message.encode(gsl::span(buffer.data(), buffer.size()));
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual == expected);
    }

    SECTION("format checksum greater than 3 digits throws") {
        REQUIRE_THROWS(message::format_checksum(9999));
    }

    SECTION("format checksum pads values with less than 3 digits") {
        REQUIRE(message::format_checksum(999) == "999");
        REQUIRE(message::format_checksum(99) == "099");
        REQUIRE(message::format_checksum(9) == "009");
        REQUIRE(message::format_checksum(0) == "000");
        REQUIRE(message::format_checksum(90) == "090");
        REQUIRE(message::format_checksum(900) == "900");
    }

}