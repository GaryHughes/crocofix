#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <iostream>

using namespace crocofix;

int main(int, char**)
{
    // Acces the definition of an enumerated field value
    auto buy = FIX_5_0SP2::field::Side::Buy;

    std::cout << buy.name() << std::endl;
    std::cout << buy.value() << std::endl;

    auto sell = FIX_5_0SP2::field::Side::Sell;

    std::cout << sell.name() << std::endl;
    std::cout << sell.value() << std::endl;

    // Iterate over all enumerated values for a field
    // TODO - make values() static
    auto side = FIX_5_0SP2::field::Side();

    for (const auto& value : side.values())
    {
        std::cout << value.get().name() << " " << value.get().value() << std::endl;
    }

    // Display field properties
    std::cout << side.tag() << '\n' 
              << side.name() << '\n'
              << side.type() << '\n' 
              << side.added() << '\n' 
              << side.synopsis() << std::endl; 

    // Is there a field defined with the specific tag? 
    std::cout << "0 - " << FIX_5_0SP2::fields()[0].is_valid() << std::endl; 
    std::cout << "54 - " << FIX_5_0SP2::fields()[54].is_valid() << std::endl;
    
    // Safely lookup the name of a field via a tag 
    std::cout << "100 - " << FIX_5_0SP2::fields().name_of_field(100) << std::endl;
    std::cout << "99999 - " << FIX_5_0SP2::fields().name_of_field(99999) << std::endl;

    // Safely lookup the name of an enumerated field value
    std::cout << "18, G = " << FIX_5_0SP2::fields().name_of_value(18, "G") << std::endl;
    std::cout << "99999, 1 = " << FIX_5_0SP2::fields().name_of_value(999999, "1") << std::endl;
    
    return 0;
}