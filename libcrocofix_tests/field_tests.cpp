#include <catch.hpp>
#include <libcrocofix/field.hpp>

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
    }

}