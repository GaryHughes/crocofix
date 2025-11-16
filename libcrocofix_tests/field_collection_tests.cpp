import crocofix;

#include <catch2/catch_all.hpp>
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
    REQUIRE(fields.set(FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::replace_first_or_append));
    REQUIRE(fields.size() == 1);    
    auto field = fields[0];
    REQUIRE(field.tag() == crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(field.value() == "ASX");
}

TEST_CASE_METHOD(fixture, "field collection add duplicate field") 
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::replace_first_or_append));
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.size() == 2);    
    auto field = fields[0];
    REQUIRE(field.tag() == crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(field.value() == "ASX");
    field = fields[1];
    REQUIRE(field.tag() == crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(field.value() == "ASX");
}

TEST_CASE_METHOD(fixture, "remove first non existent field from empty collection")
{
    REQUIRE(!fields.remove(crocofix::FIX_5_0SP2::field::ExDestination::Tag, crocofix::remove_operation::remove_first));
}

TEST_CASE_METHOD(fixture, "remove all non existent field from empty collection")
{
    REQUIRE(!fields.remove(crocofix::FIX_5_0SP2::field::ExDestination::Tag, crocofix::remove_operation::remove_all));
}

TEST_CASE_METHOD(fixture, "remove first existent field from populated collection")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.remove(crocofix::FIX_5_0SP2::field::ExDestination::Tag, crocofix::remove_operation::remove_first));
    REQUIRE(fields.size() == 1);
}

TEST_CASE_METHOD(fixture, "remove all existent field from populated collection")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.remove(crocofix::FIX_5_0SP2::field::ExDestination::Tag, crocofix::remove_operation::remove_all));
    REQUIRE(fields.empty());
}

TEST_CASE_METHOD(fixture, "get non existent field from empty collection")
{
    REQUIRE_THROWS_MATCHES(fields.get(crocofix::FIX_5_0SP2::field::TimeInForce::Tag), std::out_of_range, Catch::Matchers::Message("field collection does not contain a field with Tag 59"));
}

TEST_CASE_METHOD(fixture, "get non existent field from populated collection")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE_THROWS_MATCHES(fields.get(crocofix::FIX_5_0SP2::field::TimeInForce::Tag), std::out_of_range, Catch::Matchers::Message("field collection does not contain a field with Tag 59"));
}

TEST_CASE_METHOD(fixture, "get existent field")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.get(crocofix::FIX_5_0SP2::field::ExDestination::Tag).value() == "ASX");
}

TEST_CASE_METHOD(fixture, "try_get field from empty collection")
{
    REQUIRE(!fields.try_get(crocofix::FIX_5_0SP2::field::TimeInForce::Tag));    
}

TEST_CASE_METHOD(fixture, "try_get non existent field from populated collection")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(!fields.try_get(crocofix::FIX_5_0SP2::field::TimeInForce::Tag));    
}

TEST_CASE_METHOD(fixture, "try_get existent field")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    auto ExDestination = fields.try_get(crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(ExDestination.has_value());
    if (!ExDestination.has_value()) {
        REQUIRE(false);
        return;
    }
    REQUIRE(ExDestination->value() == "ASX");    
}

TEST_CASE_METHOD(fixture, "try_get existent field returns first instance of multiply defined field")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "TSX", set_operation::append));
    auto ExDestination = fields.try_get(crocofix::FIX_5_0SP2::field::ExDestination::Tag);
    REQUIRE(ExDestination.has_value());
    if (!ExDestination.has_value()) {
        REQUIRE(false);
        return;
    }
    REQUIRE(ExDestination->value() == "ASX");    
}

TEST_CASE_METHOD(fixture, "try_get_or_default field from empty collection")
{
    auto actual = fields.try_get_or_default(crocofix::FIX_5_0SP2::field::TimeInForce::Tag, crocofix::FIX_5_0SP2::field::TimeInForce::Day);
    REQUIRE(actual.tag() == crocofix::FIX_5_0SP2::field::TimeInForce::Tag);
    REQUIRE(actual.value() == crocofix::FIX_5_0SP2::field::TimeInForce::Day.value());
}

TEST_CASE_METHOD(fixture, "try_get_or_default non existent field from populated collection")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::ExDestination::Tag, "ASX", set_operation::append));
    auto actual = fields.try_get_or_default(crocofix::FIX_5_0SP2::field::TimeInForce::Tag, crocofix::FIX_5_0SP2::field::TimeInForce::Day);
    REQUIRE(actual.tag() == crocofix::FIX_5_0SP2::field::TimeInForce::Tag);
    REQUIRE(actual.value() == crocofix::FIX_5_0SP2::field::TimeInForce::Day.value());
}

TEST_CASE_METHOD(fixture, "try_get_or_default existent field")
{
    REQUIRE(fields.set(crocofix::FIX_5_0SP2::field::TimeInForce::Tag, crocofix::FIX_5_0SP2::field::TimeInForce::Day, set_operation::append));
    auto actual = fields.try_get_or_default(crocofix::FIX_5_0SP2::field::TimeInForce::Tag, crocofix::FIX_5_0SP2::field::TimeInForce::AtTheClose);
    REQUIRE(actual.tag() == crocofix::FIX_5_0SP2::field::TimeInForce::Tag);
    REQUIRE(actual.value() == crocofix::FIX_5_0SP2::field::TimeInForce::Day.value());
}



    // void remove_at(int index);
    // void remove_range(int first_index, int last_index);

