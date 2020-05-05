#include <libcrocofixdictionary/fix50SP2_fields.hpp>
#include <libcrocofixdictionary/fix50SP2_messages.hpp>
#include <iostream>

using namespace crocofix;

int main(int, char**)
{
    std::cout << '\n';
    std::cout << "Reference an enumerated field value directly ";
    const auto& buy = FIX_5_0SP2::field::Side::Buy;
    std::cout << buy.name() << " = " << buy.value() << "\n\n";

    std::cout << "Display a field's properties\n\n";
    auto side = FIX_5_0SP2::field::Side();
  
    std::cout << side.name() << '\n'
              << "{\n"
              << "    Tag      = " << side.tag() << '\n' 
              << "    Type     = " << side.type() << '\n' 
              << "    Added    = " << side.added() << '\n' 
              << "    Synopsis = " << side.synopsis() << '\n'
              << "}\n\n"; 

    std::cout << "Iterate over all enumerated values for a field\n\n";

    // TODO - make values() static
    std::cout << side.name() << '\n'
              << "{\n";
    for (const auto& value : side.values())
    {
        std::cout << "    " << value.get().value() << " - " << value.get().name() << '\n';
    }
    std::cout << "}\n\n";

    std::cout << "Is 0 a valid field tag? " << FIX_5_0SP2::fields()[0].is_valid() << '\n'; 
    std::cout << "Is 54 a valid field tag? " << FIX_5_0SP2::fields()[54].is_valid() << "\n\n";
    
    std::cout << "What is the name of field with tag 100? " << FIX_5_0SP2::fields().name_of_field(100) << '\n';
    std::cout << "What is the name of field with tag 99999? " << FIX_5_0SP2::fields().name_of_field(99999) << "\n\n";

    std::cout << "What is the name of enumerated field value G for field tag 18? " << FIX_5_0SP2::fields().name_of_value(18, "G") << '\n';
    std::cout << "What is the name of enumerated field value 1 for field tag 99999? " << FIX_5_0SP2::fields().name_of_value(999999, "1") << "\n\n";
    
    std::cout << "FIX_5_0SP2 has " << FIX_5_0SP2::messages().size() << " messages defined\n\n";

    std::cout << "Lookup a message by index in the messages() collection\n\n";
    
    auto heartbeat = FIX_5_0SP2::messages()[0];

    std::cout << heartbeat.name() << '\n'
              << "{\n" 
              << "    MsgType  = " << heartbeat.msg_type() << '\n'
              << "    Category = " << heartbeat.category() << '\n'
              << "    Added    = " << heartbeat.added() << '\n'        
              << "    Synopsis = " << heartbeat.synopsis() << '\n'
              << "}\n\n";
  
    std::cout << "Lookup a message by MsgType in the messages() collection\n\n";

    auto executionReport = FIX_5_0SP2::messages()["8"];

    std::cout << executionReport.name() << '\n'
              << "{\n" 
              << "    MsgType  = " << executionReport.msg_type() << '\n'
              << "    Category = " << executionReport.category() << '\n'
              << "    Added    = " << executionReport.added() << '\n'        
              << "    Synopsis = " << executionReport.synopsis() << '\n'
              << "}\n\n";

    std::cout << heartbeat.name() << " has " << heartbeat.fields().size() << " fields defined\n\n";

    std::cout << heartbeat.name() << " Fields\n{\n";

    for (const auto& field : heartbeat.fields())
    {
        std::cout << "    " << field.name() << '\n';
    }

    std::cout << "}\n\n";

    std::cout << "Lookup the name of a message via it's MsgType A = " << FIX_5_0SP2::messages().name_of_message("A") << "\n\n";

    return 0;
}