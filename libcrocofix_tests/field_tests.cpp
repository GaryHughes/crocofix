import crocofix;

#include <catch2/catch_all.hpp>

using namespace crocofix;

TEST_CASE("Field", "[field]") {

    SECTION("Timestamp format seconds") 
    {
        REQUIRE(crocofix::timestamp_string(crocofix::timestamp_format::seconds).size() == 17);
    }

    SECTION("Timestamp format milliseconds") 
    {
        REQUIRE(crocofix::timestamp_string(crocofix::timestamp_format::milliseconds).size() == 21);
    }

    SECTION("Constructors")
    {
        REQUIRE(crocofix::field(0, true).value() == "Y");    
        REQUIRE(crocofix::field(0, false).value() == "N");

        REQUIRE(crocofix::field(0, std::numeric_limits<int8_t>::min()).value() == "-128");
        REQUIRE(crocofix::field(0, std::numeric_limits<int8_t>::max()).value() == "127");

        REQUIRE(crocofix::field(0, std::numeric_limits<int16_t>::min()).value() == "-32768");
        REQUIRE(crocofix::field(0, std::numeric_limits<int16_t>::max()).value() == "32767");

        REQUIRE(crocofix::field(0, std::numeric_limits<int32_t>::min()).value() == "-2147483648");
        REQUIRE(crocofix::field(0, std::numeric_limits<int32_t>::max()).value() == "2147483647");

        REQUIRE(crocofix::field(0, std::numeric_limits<int64_t>::min()).value() == "-9223372036854775808");
        REQUIRE(crocofix::field(0, std::numeric_limits<int64_t>::max()).value() == "9223372036854775807");

        REQUIRE(crocofix::field(0, std::numeric_limits<uint8_t>::min()).value() == "0");
        REQUIRE(crocofix::field(0, std::numeric_limits<uint8_t>::max()).value() == "255");

        REQUIRE(crocofix::field(0, std::numeric_limits<uint16_t>::min()).value() == "0");
        REQUIRE(crocofix::field(0, std::numeric_limits<uint16_t>::max()).value() == "65535");

        REQUIRE(crocofix::field(0, std::numeric_limits<uint32_t>::min()).value() == "0");
        REQUIRE(crocofix::field(0, std::numeric_limits<uint32_t>::max()).value() == "4294967295");

        REQUIRE(crocofix::field(0, std::numeric_limits<uint64_t>::min()).value() == "0");
        REQUIRE(crocofix::field(0, std::numeric_limits<uint64_t>::max()).value() == "18446744073709551615");
    }
}