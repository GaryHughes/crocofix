#include <catch.hpp>
// #include <libcrocofixdictionary/version_field.hpp>
// #include <libcrocofixdictionary/version_field_collection.hpp>
// #include <libcrocofixdictionary/value.hpp>
#include <libcrocofixdictionary/fix44_version_fields.hpp>

// This is hand written to figure out what we will generate with the lexicographer
// namespace crocofix::FIX_4_4
// {

// class Side : public crocofix::dictionary::version_field
// {
// public:

//     static constexpr crocofix::dictionary::field_value Buy = crocofix::dictionary::field_value("Buy", "1");
//     static constexpr crocofix::dictionary::field_value Sell = crocofix::dictionary::field_value("Sell", "2");

//     // TODO - replace type and version with actual types
//     Side()
//     :   crocofix::dictionary::version_field(54, "Side", "SideCodeSet", "FIX.2.7", "SYNOPSIS")
//     {
//     }

// };

// const crocofix::dictionary::version_field_collection& fields() noexcept;

// const crocofix::dictionary::version_field_collection& fields() noexcept
// {
//     static crocofix::dictionary::version_field_collection fields = {
//         Side()
//     };

//     return fields; 
// }

// }

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

        // REQUIRE(crocofix::FIX_4_4::fields()[0].is_valid() == false);
        // REQUIRE(crocofix::FIX_4_4::fields().size() == 55);
        // REQUIRE(crocofix::FIX_4_4::fields()[54].is_valid() == true);
        // REQUIRE(crocofix::FIX_4_4::fields()[54].tag() == 54);

        // for (const auto& field : crocofix::FIX_4_4::fields())
        // {

        // }

    }
  
}