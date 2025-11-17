module;

#include <cstdint>

export module crocofix.utility:number_of_digits;

export namespace crocofix
{

uint32_t number_of_digits(uint64_t value);
uint32_t number_of_digits(int value);

}
