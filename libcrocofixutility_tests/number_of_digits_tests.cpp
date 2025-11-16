import crocofix.utility;

#include <catch2/catch_all.hpp>

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
        REQUIRE(number_of_digits(uint64_t(1'000'000'000)) == 10);
        REQUIRE(number_of_digits(uint64_t(9'999'999'999)) == 10);
        REQUIRE(number_of_digits(uint64_t(10'000'000'000)) == 11);
        REQUIRE(number_of_digits(uint64_t(99'999'999'999)) == 11);
        REQUIRE(number_of_digits(uint64_t(100'000'000'000)) == 12);
        REQUIRE(number_of_digits(uint64_t(999'999'999'999)) == 12);
        REQUIRE(number_of_digits(uint64_t(1'000'000'000'000)) == 13);
        REQUIRE(number_of_digits(uint64_t(9'999'999'999'999)) == 13);
        REQUIRE(number_of_digits(uint64_t(10'000'000'000'000)) == 14);
        REQUIRE(number_of_digits(uint64_t(99'999'999'999'999)) == 14);
        REQUIRE(number_of_digits(uint64_t(100'000'000'000'000)) == 15);
        REQUIRE(number_of_digits(uint64_t(999'999'999'999'999)) == 15);
        REQUIRE(number_of_digits(uint64_t(1'000'000'000'000'000)) == 16);
        REQUIRE(number_of_digits(uint64_t(9'999'999'999'999'999)) == 16);
        REQUIRE(number_of_digits(uint64_t(10'000'000'000'000'000)) == 17);
        REQUIRE(number_of_digits(uint64_t(99'999'999'999'999'999)) == 17);
        REQUIRE(number_of_digits(uint64_t(100'000'000'000'000'000)) == 18);
        REQUIRE(number_of_digits(uint64_t(999'999'999'999'999'999)) == 18);
        REQUIRE(number_of_digits(uint64_t(1'000'000'000'000'000'000)) == 19);
        REQUIRE(number_of_digits(uint64_t(9'999'999'999'999'999'999UL)) == 19);
        REQUIRE(number_of_digits(uint64_t(10'000'000'000'000'000'000UL)) == 20);
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