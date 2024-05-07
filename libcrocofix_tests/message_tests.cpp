#include <catch2/catch_all.hpp>
#include <libcrocofix/message.hpp>

using namespace crocofix;

TEST_CASE("Message", "[message]") {

    // TODO - checksum in decode tests

    SECTION("Decode a complete message") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto [consumed, complete] = message.decode(text);
        REQUIRE(complete);
        REQUIRE(consumed == text.length());
        REQUIRE(message.fields().size() == 18);
    }

    SECTION("Decode a complete message in two pieces aligned on a field boundary") 
    {
        const std::string one = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u0001";
        const std::string two = "54=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto one_result = message.decode(one);
        REQUIRE_FALSE(one_result.complete);
        REQUIRE(one_result.consumed == one.length());
        auto two_result = message.decode(two);
        REQUIRE(two_result.complete);
        REQUIRE(two_result.consumed == two.length());
        REQUIRE(message.fields().size() == 18);
    }

    SECTION("Decode a complete message in two pieces not aligned on a field boundary") 
    {
        const std::string one = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=B";
        const std::string two = "55=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        auto one_result = message.decode(one);
        REQUIRE_FALSE(one_result.complete);
        REQUIRE(one_result.consumed == one.length() - std::string("55=B").length());
        auto two_result = message.decode(two);
        REQUIRE(two_result.complete);
        REQUIRE(two_result.consumed == two.length());
        REQUIRE(message.fields().size() == 18);
    }
  
    SECTION("Invalid tag throws") 
    {
        crocofix::message message;
        REQUIRE_THROWS_AS(message.decode("A=FIX.4.4"), std::out_of_range);
    }

    SECTION("MsgType lookup throws for a message with no MsgType") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE_THROWS(message.MsgType());
    }

    SECTION("MsgType lookup") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE(message.MsgType() == "D");
    }

    SECTION("is_admin is false for a non admin message") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE(message.is_admin() == false);        
    }

    SECTION("is_admin is true for an admin message") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=A\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(text);
        REQUIRE(message.is_admin() == true);        
    }

    SECTION("encode a message") 
    {
        const std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(expected);

        std::array<char, 1024> buffer {};
        auto result = message.encode(std::span(buffer.data(), buffer.size()));
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual.compare(0, std::string::npos, expected) == 0);
    }

    SECTION("encode does not add CheckSum if it is not present") 
    {
        const std::string expected = "8=FIX.4.4\u00019=149\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u0001";
        crocofix::message message;
        message.decode(expected);

        std::array<char, 1024> buffer {};
        auto result = message.encode(std::span(buffer.data(), buffer.size()));
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual.compare(0, std::string::npos, expected) == 0);
    }

    SECTION("encode does not add BodyLength if it is not present") 
    {
        const std::string expected = "8=FIX.4.4\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        message.decode(expected);

        std::array<char, 1024> buffer {};
        auto result = message.encode(std::span(buffer.data(), buffer.size()), crocofix::encode_options::standard & ~crocofix::encode_options::set_checksum);
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual.compare(0, std::string::npos, expected) == 0);
    }

    SECTION("format checksum greater than 3 digits throws") 
    {
        REQUIRE_THROWS(message::format_checksum(9999));
    }

    SECTION("format checksum pads values with less than 3 digits") 
    {
        REQUIRE(message::format_checksum(999) == "999");
        REQUIRE(message::format_checksum(99) == "099");
        REQUIRE(message::format_checksum(9) == "009");
        REQUIRE(message::format_checksum(0) == "000");
        REQUIRE(message::format_checksum(90) == "090");
        REQUIRE(message::format_checksum(900) == "900");
    }

    SECTION("decode a message with a data field that has not preceeding size field") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000189=123\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        REQUIRE_THROWS(message.decode(text));
    }

    SECTION("decode a message with a non numeric previous field value") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000189=123\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        REQUIRE_THROWS(message.decode(text));
    }

    SECTION("decode a message with a data field that does not have a traling field separator") 
    {
        const std::string text = "8=FIX.4.4\u00019=149\u000135=D\u000193=3\u000189=AAA49=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000110=021\u0001";
        crocofix::message message;
        REQUIRE_THROWS(message.decode(text));
    }

    SECTION("decode a message containing a data field") 
    {
        const std::string signature = "ABCDEF\u0001ABCDEFABC\u0001DEF";
        const std::string text = "8=FIX.4.4\u00019=167\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000193=20\u000189=ABCDEF\u0001ABCDEFABC\u0001DEF\u000110=220\u0001";
        crocofix::message message;
        auto [_, complete] = message.decode(text);
        REQUIRE(complete);
        REQUIRE(message.fields().size() == 20);
        REQUIRE(message.fields()[18].value().size() == signature.size());
        REQUIRE(signature.compare(0, std::string::npos, message.fields()[18].value()) == 0);
    }

    SECTION("decode a message containing a data field in two pieces") 
    {
        const std::string signature = "ABCDEF\u0001ABCDEFABC\u0001DEF";
        const std::string one = "8=FIX.4.4\u00019=167\u000135=D\u000149=INITIATOR\u000156=ACCEPTOR\u000134=2752\u000152=20200114-08:13:20.041\u000111=61\u000170=60\u0001100=AUTO\u000155=BHP.AX\u000154=1\u000160=20200114-08:12:59.397\u000138=10000\u000140=2\u000144=20\u000159=1\u000193=20\u000189=ABCDEF\u0001ABCDE";
        const std::string two = "89=ABCDEF\u0001ABCDEFABC\u0001DEF\u000110=220\u0001";
        crocofix::message message;
        auto one_result = message.decode(one);
        REQUIRE_FALSE(one_result.complete);
        REQUIRE(one_result.consumed == one.length() - std::string("89=ABCDEF\u0001ABCDE").length());
        REQUIRE(message.fields().size() == 18);
        auto two_result = message.decode(two);
        REQUIRE(two_result.complete);
        REQUIRE(message.fields().size() == 20);
        REQUIRE(message.fields()[18].value().size() == signature.size());
        REQUIRE(signature.compare(0, std::string::npos, message.fields()[18].value()) == 0);
    }


    SECTION("encode a message containing a data field") 
    {
        const std::string expected = "8=FIX.4.4\u00019=30\u000193=20\u000189=ABCDEF\u0001ABCDEFABC\u0001DEF\u000110=119\u0001";

        crocofix::message message;
        message.fields().emplace_back(8, "FIX.4.4");
        message.fields().emplace_back(9, "40");
        message.fields().emplace_back(93, "20");
        message.fields().emplace_back(89, "ABCDEF\u0001ABCDEFABC\u0001DEF");
        message.fields().emplace_back(10, "220");

        std::array<char, 1024> buffer {};
        auto result = message.encode(std::span(buffer.data(), buffer.size()));
        REQUIRE(result > 0);
        auto actual = std::string_view(buffer.data(), result);
        REQUIRE(actual.compare(0, std::string::npos, expected) == 0);
    };

}