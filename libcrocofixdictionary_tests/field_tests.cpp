#include <catch.hpp>
#include <libcrocofixdictionary/fix44_version_fields.hpp>

using namespace crocofix;

TEST_CASE("field", "[field]") {

    SECTION("Value Definitions") {

        auto side = FIX_4_4::Side::Buy;

        REQUIRE(side.name() == "Buy");
        REQUIRE(side.value() == "1");

        side = FIX_4_4::Side::Sell;

        REQUIRE(side.name() == "Sell");
        REQUIRE(side.value() == "2");

    }

    SECTION("Version Field Definitions") {

        REQUIRE(crocofix::FIX_4_4::fields()[0].is_valid() == false);
        REQUIRE(crocofix::FIX_4_4::fields().size() == 957);
        REQUIRE(crocofix::FIX_4_4::fields()[54].is_valid() == true);
        REQUIRE(crocofix::FIX_4_4::fields()[54].tag() == 54);

    }
  
}