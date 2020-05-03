#include <catch.hpp>
#include <libcrocofixutility/number_of_digits.hpp>

using namespace crocofix;

TEST_CASE("number_of_digits", "[number_of_digits]") {

    SECTION("number of digits in positive integers") {

        REQUIRE(number_of_digits(0) == 1);
        REQUIRE(number_of_digits(9) == 1);
        REQUIRE(number_of_digits(10) == 2);
        REQUIRE(number_of_digits(99) == 2);
        REQUIRE(number_of_digits(100) == 3);
        REQUIRE(number_of_digits(999) == 3);
        REQUIRE(number_of_digits(1'000) == 4);
        REQUIRE(number_of_digits(9'999) == 4);
        REQUIRE(number_of_digits(10'000) == 5);
        REQUIRE(number_of_digits(99'999) == 5);
        REQUIRE(number_of_digits(100'000) == 6);
        REQUIRE(number_of_digits(999'999) == 6);
        REQUIRE(number_of_digits(1'000'000) == 7);
        REQUIRE(number_of_digits(9'999'999) == 7);
        REQUIRE(number_of_digits(10'000'000) == 8);
        REQUIRE(number_of_digits(99'999'999) == 8);
        REQUIRE(number_of_digits(100'000'000) == 9);
        REQUIRE(number_of_digits(999'999'999) == 9);
        REQUIRE(number_of_digits(1'000'000'000) == 10);
        REQUIRE(number_of_digits(9'999'999'999ull) == 10);
        REQUIRE(number_of_digits(10'000'000'000ull) == 11);
        REQUIRE(number_of_digits(99'999'999'999ull) == 11);
        REQUIRE(number_of_digits(100'000'000'000ull) == 12);
        REQUIRE(number_of_digits(999'999'999'999ull) == 12);
        REQUIRE(number_of_digits(1'000'000'000'000ull) == 13);
        REQUIRE(number_of_digits(9'999'999'999'999ull) == 13);
        REQUIRE(number_of_digits(10'000'000'000'000ull) == 14);
        REQUIRE(number_of_digits(99'999'999'999'999ull) == 14);
        REQUIRE(number_of_digits(100'000'000'000'000ull) == 15);
        REQUIRE(number_of_digits(999'999'999'999'999ull) == 15);
        REQUIRE(number_of_digits(1'000'000'000'000'000ull) == 16);
        REQUIRE(number_of_digits(9'999'999'999'999'999ull) == 16);
        REQUIRE(number_of_digits(10'000'000'000'000'000ull) == 17);
        REQUIRE(number_of_digits(99'999'999'999'999'999ull) == 17);
        REQUIRE(number_of_digits(100'000'000'000'000'000ull) == 18);
        REQUIRE(number_of_digits(999'999'999'999'999'999ull) == 18);
        REQUIRE(number_of_digits(1'000'000'000'000'000'000ull) == 19);
        REQUIRE(number_of_digits(9'999'999'999'999'999'999ull) == 19);
        REQUIRE(number_of_digits(10'000'000'000'000'000'000ull) == 20);
    }

    SECTION("number of digits in negative integers") {

        REQUIRE(number_of_digits(-9) == 1);
        REQUIRE(number_of_digits(-10) == 2);
        REQUIRE(number_of_digits(-99) == 2);
        REQUIRE(number_of_digits(-100) == 3);
        REQUIRE(number_of_digits(-999) == 3);
        REQUIRE(number_of_digits(-1'000) == 4);
        REQUIRE(number_of_digits(-9'999) == 4);
        REQUIRE(number_of_digits(-10'000) == 5);
        REQUIRE(number_of_digits(-99'999) == 5);
        REQUIRE(number_of_digits(-100'000) == 6);
        REQUIRE(number_of_digits(-999'999) == 6);
        REQUIRE(number_of_digits(-1'000'000) == 7);
        REQUIRE(number_of_digits(-9'999'999) == 7);
        REQUIRE(number_of_digits(-10'000'000) == 8);
        REQUIRE(number_of_digits(-99'999'999) == 8);
        REQUIRE(number_of_digits(-100'000'000) == 9);
        REQUIRE(number_of_digits(-999'999'999) == 9);
        REQUIRE(number_of_digits(-1'000'000'000) == 10);
    
    }

}