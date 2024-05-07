#include <catch2/catch_all.hpp>
#include <libcrocofixdictionary/fix44_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using namespace crocofix;

TEST_CASE("field", "[field]") {

    SECTION("Value Definitions") {

        auto side = FIX_4_4::field::Side::Buy;

        REQUIRE(side.name() == "Buy");
        REQUIRE(side.value() == "1");

        side = FIX_4_4::field::Side::Sell;

        REQUIRE(side.name() == "Sell");
        REQUIRE(side.value() == "2");

    }

    SECTION("Version Field Definitions") {

        REQUIRE(crocofix::FIX_4_4::fields()[0].is_valid() == false);
        REQUIRE(crocofix::FIX_4_4::fields().size() == 913);
        REQUIRE(crocofix::FIX_4_4::fields()[54].is_valid() == true);
        REQUIRE(crocofix::FIX_4_4::fields()[54].tag() == 54);

    }

    SECTION("Lookup field name") {
        REQUIRE(crocofix::FIX_4_4::fields().name_of_field(100) == "ExDestination");
        REQUIRE(crocofix::FIX_4_4::fields().name_of_field(999999).empty());
    }

    SECTION("Lookup field value") {
        REQUIRE(crocofix::FIX_4_4::fields().name_of_value(18, "G") == "AllOrNone");
        REQUIRE(crocofix::FIX_4_4::fields().name_of_value(999999, "1").empty());
        REQUIRE(crocofix::FIX_4_4::fields().name_of_value(999999, "54").empty());
    }
  
    SECTION("test") {
        REQUIRE(crocofix::FIX_5_0SP2::fields().name_of_value(18, "G") == "AllOrNone");
        REQUIRE(crocofix::FIX_5_0SP2::fields().name_of_value(999999, "1").empty());
        REQUIRE(crocofix::FIX_5_0SP2::fields().name_of_value(999999, "54").empty());

        REQUIRE(crocofix::FIX_5_0SP2::fields()[1].name() == "Account");
    }

    SECTION("tag too high throws") {
        REQUIRE_THROWS_AS(crocofix::FIX_4_4::fields()[1000], std::out_of_range);
    }

    SECTION("Lookup field by name") {
        REQUIRE(crocofix::FIX_5_0SP2::fields()["ExDestination"].tag() == 100);
    }

    SECTION("Lookup invalid name throws") {
        REQUIRE_THROWS_AS(crocofix::FIX_5_0SP2::fields()["MadeUp"], std::out_of_range);
    }
}