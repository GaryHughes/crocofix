#include <catch.hpp>
#include <libcrocofix/field_collection.hpp>
#include <libcrocofixdictionary/fix50SP2_fields.hpp>

using namespace crocofix;

struct fixture
{
    crocofix::field_collection fields;
};

TEST_CASE_METHOD(fixture, "field collection default state") 
{
    REQUIRE(fields.empty());    
}

TEST_CASE_METHOD(fixture, "field collection overwrite non existent field") 
{
    fields.set(FIX_5_0SP2::field::ExDestination::Tag, "ASX");
    REQUIRE(fields.empty());    
}

TEST_CASE_METHOD(fixture, "field collection add non existent field") 
{
    REQUIRE(fields.set(FIX_5_0SP2::field::ExDestination::Tag, "ASX", field_operation::replace_first_or_append));
    REQUIRE(fields.size() == 1);    
    auto field = fields[0];
    REQUIRE(field.tag() == crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(field.value() == "ASX");
}

TEST_CASE_METHOD(fixture, "field collection add duplicate field") 
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", field_operation::replace_first_or_append));
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", field_operation::append));
    REQUIRE(fields.size() == 2);    
    auto field = fields[0];
    REQUIRE(field.tag() == crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(field.value() == "ASX");
    field = fields[1];
    REQUIRE(field.tag() == crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(field.value() == "ASX");
}



    // bool set(int tag, std::string value, bool add_if_missing = false);
    // bool set(int tag, uint32_t value, bool add_if_missing = false);
    // bool set(int tag, uint64_t value, bool add_if_missing = false);
    // bool set(int tag, bool value, bool add_if_missing = false);
    // bool set(const field& field, bool add_if_missing = false);

    // std::optional<field> try_get(int tag) const noexcept;

    // void remove(int tag); // remove_first
    // void remove_at(int index);
    // void remove_range(int first_index, int last_index);

