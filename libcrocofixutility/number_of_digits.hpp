#ifndef crocofix_utilities_number_of_digits_hpp
#define crocofix_utilities_number_of_digits_hpp

#include <iostream>
#include <functional>

namespace crocofix
{

uint32_t number_of_digits(uint64_t value) 
{
    return  1
        + (std::uint32_t)(value >= 10)
        + (std::uint32_t)(value >= 100)
        + (std::uint32_t)(value >= 1000)
        + (std::uint32_t)(value >= 10000)
        + (std::uint32_t)(value >= 100000)
        + (std::uint32_t)(value >= 1000000)
        + (std::uint32_t)(value >= 10000000)
        + (std::uint32_t)(value >= 100000000)
        + (std::uint32_t)(value >= 1000000000)
        + (std::uint32_t)(value >= 10000000000ull)
        + (std::uint32_t)(value >= 100000000000ull)
        + (std::uint32_t)(value >= 1000000000000ull)
        + (std::uint32_t)(value >= 10000000000000ull)
        + (std::uint32_t)(value >= 100000000000000ull)
        + (std::uint32_t)(value >= 1000000000000000ull)
        + (std::uint32_t)(value >= 10000000000000000ull)
        + (std::uint32_t)(value >= 100000000000000000ull)
        + (std::uint32_t)(value >= 1000000000000000000ull)
        + (std::uint32_t)(value >= 10000000000000000000ull);
}

uint32_t number_of_digits(int value)
{
    return number_of_digits(static_cast<uint64_t>(std::abs(value)));
}

}

#endif