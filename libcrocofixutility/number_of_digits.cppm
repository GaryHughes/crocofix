module;

#include <cstdint>
#include <cmath>

export module crocofix.utility:number_of_digits;

export namespace crocofix
{

constexpr uint32_t number_of_digits(uint64_t value)
{
    return  1
        + static_cast<uint32_t>(value >= 10)
        + static_cast<uint32_t>(value >= 100)
        + static_cast<uint32_t>(value >= 1000)
        + static_cast<uint32_t>(value >= 10000)
        + static_cast<uint32_t>(value >= 100000)
        + static_cast<uint32_t>(value >= 1000000)
        + static_cast<uint32_t>(value >= 10000000)
        + static_cast<uint32_t>(value >= 100000000)
        + static_cast<uint32_t>(value >= 1000000000)
        + static_cast<uint32_t>(value >= 10000000000ULL)
        + static_cast<uint32_t>(value >= 100000000000ULL)
        + static_cast<uint32_t>(value >= 1000000000000ULL)
        + static_cast<uint32_t>(value >= 10000000000000ULL)
        + static_cast<uint32_t>(value >= 100000000000000ULL)
        + static_cast<uint32_t>(value >= 1000000000000000ULL)
        + static_cast<uint32_t>(value >= 10000000000000000ULL)
        + static_cast<uint32_t>(value >= 100000000000000000ULL)
        + static_cast<uint32_t>(value >= 1000000000000000000ULL)
        + static_cast<uint32_t>(value >= 10000000000000000000ULL);
}

constexpr uint32_t number_of_digits(int value)
{
    return number_of_digits(static_cast<uint64_t>(std::abs(value)));
}

}
